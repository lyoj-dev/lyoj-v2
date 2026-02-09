/**
 * @file judge.cpp
 * @author LittleYang0531 (littleyang0531@cau.edu.cn)
 * @brief 评测机服务主文件
 * @version 1.0.0
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2025 LittleYang0531
 * 
 */

#include <pthread.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<arpa/inet.h>
#include"../shared/json.h"
#include"../shared/log.h"
#include"../shared/socket.h"
#include"../shared/type.h"
#include"../shared/mysql.h"
#include"../shared/utils.h"
#include"../shared/proc.h"

Json::Value judge;
auto defaultResult = [](int lang){
    Json::Value res;
    res["status"] = Waiting;
    res["type"] = DetailedSubmission;
    res["score"] = 0;
    res["time"] = 0;
    res["memory"] = 0;
    res["langCode"] = lang;
    res["langMode"] = judge["languages"][lang]["mode"];
    res["langName"] = judge["languages"][lang]["name"];
    res["subtasks"].resize(0);
    return res;
};

bool isJudging[1024];
std::set<int> judging;
MYSQL mysql;
std::map<int, Json::Value> results;
std::map<int, std::set<int> > requestIds; // 通过 connId 查 submitId
std::map<int, std::set<int> > submissionIds; // 通过 submitId 查 connId;
std::map<int, Connection> conns;
int connsCount = 0;
struct Args {
    int id; // 临时目录 id
    int sid; // 提交 id
    int cid; // 比赛 id
    int uid; // 用户 id
    int pid; // 题目 id
    std::string cmd; // 启动单元程序指令
    MYSQL mysql; // mysql 连接符
};
void* unit_work_thread(void* arg) {
    proc_settitle("lyoj-judge: unit worker process");
    system((*(std::string*)arg).c_str());
    return NULL;
}
void* work_thread(void* arg) {
    proc_settitle("lyoj-judge: worker process");
    auto args = *(Args*)arg;
    delete (Args*)arg;
    Server server("../tmp/" + std::to_string(args.id) + "/server.sock");
    pid_t pt;
    proc_create(&pt, unit_work_thread, (void*)&args.cmd);
    Connection conn = server.accept();
    Json::Value &result = results[args.sid];
    int score = 0, testing = 0;
    int64_t time = 0, memory = 0;
    while (1) {
        std::string msg = conn.recv();
        Json::Value val = json_decode(msg);
        switch(val["type"].asInt()) {
            case DetailedSubmission: {
                result = val, result.removeMember("type");
                val["sid"] = args.sid;
                for (auto connId : submissionIds[args.sid]) conns[connId].send(json_encode(val));
            }; break;
            case UpdateSubmission: {
                result["info"] = val["info"];
                result["score"] = val["score"];
                result["time"] = val["time"];
                result["memory"] = val["memory"];
                result["status"] = val["status"];
                // 先发一次包保证所有测试点状态都被更新
                Json::Value tmp = result;
                tmp["type"] = DetailedSubmission;
                tmp["sid"] = args.sid;
                for (auto connId : submissionIds[args.sid]) conns[connId].send(json_encode(tmp));
                // 再发一次包来表示结束
                val["sid"] = args.sid;
                for (auto connId : submissionIds[args.sid]) {
                    conns[connId].send(json_encode(val));
                    requestIds[connId].erase(requestIds[connId].find(args.sid));
                    if (requestIds[connId].size() == 0) {
                        conns.erase(conns.find(connId));
                        requestIds.erase(requestIds.find(connId));
                    }
                }
                submissionIds.erase(submissionIds.find(args.sid));
            }; break;
            case UpdateSubtask: {
                result["subtasks"][val["subtaskId"].asInt()]["score"] = val["score"];
                result["subtasks"][val["subtaskId"].asInt()]["status"] = val["status"];
                score += val["score"].asInt();
                result["score"] = score;

                val["sid"] = args.sid;
                val["totalScore"] = score;
                for (auto connId : submissionIds[args.sid]) conns[connId].send(json_encode(val));
            }; break;
            case UpdateData: {
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["info"] = val["info"];
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["memory"] = val["memory"];
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["time"] = val["time"];
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["score"] = val["score"];
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["status"] = val["status"];
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["output"] = val["output"];
                result["subtasks"][val["subtaskId"].asInt()]["datas"][val["dataId"].asInt()]["outputIgnored"] = val["outputIgnored"];
                if (val["subtaskId"].asInt() == 0) score += val["score"].asInt();
                time += val["time"].asInt64();
                memory = std::max(memory, val["memory"].asInt64());
                result["score"] = score;
                result["time"] = time;
                result["memory"] = memory;
                if (val["status"].asInt() <= CE) testing++;

                val["sid"] = args.sid;
                val["totalScore"] = score;
                val["totalTime"] = time;
                val["totalMemory"] = memory;
                val["totalStatus"] = "Running on case " + std::to_string(testing + 1) + "...";
                for (auto connId : submissionIds[args.sid]) conns[connId].send(json_encode(val));
            }; break;
        }
        if (val["type"] == UpdateSubmission || val["type"] == DetailedSubmission && val["status"] <= CE) break;
    }
    mysqli_execute(
        args.mysql, 
        "UPDATE submission SET result = \"%s\", status = %d, score = %d, judged = true WHERE id = %d AND contest = %d",
        quote_encode(json_encode(result)).c_str(),
        result["status"].asInt(),
        result["score"].asInt(),
        args.sid,
        args.cid
    );
    if (args.cid) {
        auto res = mysqli_query(
            args.mysql,
            "SELECT * FROM contest_ranking WHERE id = %d AND uid = %d",
            args.cid,
            args.uid
        );
        auto contest = mysqli_query(
            args.mysql,
            "SELECT type, starttime FROM contest WHERE id = %d",
            args.cid
        );
        int type = contest.size() == 0 ? OI : atoi(contest[0]["type"].c_str());
        time_t submitTime = atol(mysqli_query(mysql, "SELECT time FROM submission WHERE id = %d", args.sid)[0]["time"].c_str());
        if (res.size()) {
            int score = atoi(res[0]["score"].c_str());
            int time1 = atoi(res[0]["time"].c_str());
            int time2 = atoi(res[0]["time2"].c_str());
            int penalty = atoi(res[0]["penalty"].c_str());
            auto info = json_decode(res[0]["info"]);
            for (int i = 0; i < info.size(); i++) {
                if (info[i]["pid"].asInt() != args.pid) continue;
                if (type == ACM) result["score"] = result["status"].asInt() == AC ? 1 : 0;
                // 更新分数
                if (info[i]["score"].asInt() < result["score"].asInt()) {
                    score += result["score"].asInt() - info[i]["score"].asInt();
                    info[i]["score"] = result["score"].asInt();
                    time1 += time - info[i]["time"].asInt();
                    info[i]["time"] = time;
                    info[i]["sid"] = args.sid;
                }
                // 更新罚时
                if (info[i]["time2"].asInt() == 0 && result["status"].asInt() != AC)
                    info[i]["penalty"] = info[i]["penalty"].asInt() + 1;
                // 更新 sid
                if (info[i]["score"].asInt() == result["score"].asInt()) 
                    info[i]["sid"] = args.sid;
                // 更新 time
                if (info[i]["score"].asInt() == result["score"].asInt() && time < info[i]["time"].asInt()) {
                    time1 += time - info[i]["time"].asInt();
                    info[i]["time"] = time;
                }
                // 更新 time2
                if (info[i]["time2"].asInt() == 0 && result["status"].asInt() == AC) {
                    time2 += submitTime - atol(contest[0]["starttime"].c_str());
                    penalty += info[i]["penalty"].asInt();
                    info[i]["time2"] = submitTime - atol(contest[0]["starttime"].c_str());
                }

                if (type == OI) {
                    score += result["score"].asInt() - info[i]["score"].asInt();
                    info[i]["score"] = result["score"];
                    info[i]["sid"] = args.sid;
                    time1 += time - info[i]["time"].asInt();
                    info[i]["time"] = time;
                }

                mysqli_execute(
                    args.mysql,
                    "UPDATE contest_ranking SET score = %d, time = %d, time2 = %d, penalty = %d, info = \"%s\" WHERE id = %d AND uid = %d",
                    score,
                    time1,
                    time2,
                    penalty,
                    quote_encode(json_encode(info)).c_str(),
                    args.cid,
                    args.uid
                );
                break;
            }
        }
    }
    mysqli_close(args.mysql);
    isJudging[args.id] = false;
    judging.erase(judging.find(args.sid));
    return NULL;
}

MYSQL quick_mysqli_connect() {
    return mysqli_connect(
        judge["mysql"]["server"].asString(),
        judge["mysql"]["user"].asString(),
        judge["mysql"]["passwd"].asString(),
        judge["mysql"]["database"].asString(),
        judge["mysql"]["port"].asInt()
    );
}
void* webserver_work_thread(void* arg) {
    proc_settitle("lyoj-judge: socket worker process");
    Connection conn = *(Connection*)arg;
    delete (Connection*)arg;
    Json::Value data = json_decode(conn.recv());
    if (data["type"].asInt() == SimpleSubmissions) {
        std::string ids = "";
        for (int i = 0; i < data["items"].size(); i++) ids += (i ? "," : "") + data["items"][i].asString();
        MYSQL mysql = quick_mysqli_connect();
        auto res = mysqli_query(
            mysql,
            "SELECT id, status, score, judged, lang FROM submission WHERE id in (%s)",
            ids.c_str()
        );
        Json::Value object;
        for (int i = 0; i < data["items"].size(); i++) {
            Json::Value single;
            single["id"] = data["items"][i].asInt();
            for (int j = 0; j < res.size(); j++) if (res[j]["id"] == data["items"][i].asString()) {
                if (results.find(data["items"][i].asInt()) == results.end()) 
                    results[data["items"][i].asInt()] = defaultResult(atoi(res[j]["lang"].c_str()));
                single["judged"] = atoi(res[j]["judged"].c_str()) ? true : false;
                single["score"] = 
                    single["judged"].asBool() ? 
                    atoi(res[j]["score"].c_str()) : 
                    results[data["items"][i].asInt()]["score"].asInt();
                single["statusType"] = 
                    single["judged"].asBool() ? 
                    atoi(res[j]["status"].c_str()) : 
                    results[data["items"][i].asInt()]["status"].asInt();
                single["status"] = 
                    single["statusType"].asInt() == Running ? 
                    sprintf(JudgeResultInfo[Running].c_str(), [&](){
                        auto result = results[data["items"][i].asInt()];
                        int sum = 0;
                        for (int i = 0; i < result["subtasks"].size(); i++) {
                            for (int j = 0; j < result["subtasks"][i]["datas"].size(); j++) {
                                sum += result["subtasks"][i]["datas"][j]["status"].asInt() <= CE;
                            }
                        }
                        return sum;
                    }()) :
                    JudgeResultInfo[single["statusType"].asInt()];
            }
            object.append(single);
        }
        conn.send(json_encode(object));
        mysqli_close(mysql);
    }
    else if (data["type"].asInt() == SimpleSubmissionsLongConnection) {
        int connId = connsCount++;
        conns[connId] = conn;
        for (int i = 0; i < data["items"].size(); i++) {
            int submitId = data["items"][i].asInt();
            requestIds[connId].insert(submitId);
            submissionIds[submitId].insert(connId);
        }
    }
    else if (data["type"].asInt() == DetailedSubmission) {
        MYSQL mysql = quick_mysqli_connect();
        auto res = mysqli_query(
            mysql,
            "SELECT * FROM submission WHERE id = %d",
            data["item"].asInt()
        );
        Json::Value result;
        if (atoi(res[0]["judged"].c_str()) == 0) {
            if (results.find(data["item"].asInt()) == results.end()) 
                results[data["item"].asInt()] = defaultResult(atoi(res[0]["lang"].c_str()));
            result = results[data["item"].asInt()];
        } else result = json_decode(res[0]["result"]);
        result["statusType"] = result["status"];
        result["status"] = 
            result["statusType"].asInt() == Running ? 
            sprintf(JudgeResultInfo[Running].c_str(), [&](){
                int sum = 0;
                for (int i = 0; i < result["subtasks"].size(); i++) {
                    for (int j = 0; j < result["subtasks"][i]["datas"].size(); j++) {
                        sum += result["subtasks"][i]["datas"][j]["status"].asInt() <= CE;
                    }
                }
                return sum;
            }()) :
            JudgeResultInfo[result["statusType"].asInt()];
        conn.send(json_encode(result));
        mysqli_close(mysql);
    }
    else if (data["type"].asInt() == DetailedSubmissionLongConnection) {
        Json::Value res = results[data["item"].asInt()];
        res["sid"] = data["item"].asInt();
        res["type"] = DetailedSubmission;
        conn.send(json_encode(res));
        int connId = connsCount++;
        conns[connId] = conn;
        int submitId = data["item"].asInt();
        requestIds[connId].insert(submitId);
        submissionIds[submitId].insert(connId);
    }
    return NULL;
}
void* socket_work_thread(void* arg) {
    proc_settitle("lyoj-judge: socket master process");
    Server server(judge["sock"].asString());
    writeLog(LOG_LEVEL_INFO, "Listening to web server connection...");
    while (1) {
        Connection *conn = new Connection;
        *conn = server.accept();
        pid_t pt;
        proc_create(&pt, webserver_work_thread, (void*)conn);
    }
    return NULL;
}

int main(int argc, char** argv) {
    proc_inittitle(argv);
    proc_settitle("lyoj-judge: master process");

    judge = json_decode(readFile("./judge.json"));
    for (int i = 1; i <= judge["thread"].asInt(); i++)
        mkdir(("../tmp/" + std::to_string(i)).c_str(), 0777),
        chmod(("../tmp/" + std::to_string(i)).c_str(), 0777);
    mysql = quick_mysqli_connect();
    writeLog(LOG_LEVEL_INFO, "Listening to mysql server...");

    pid_t pt;
    proc_create(&pt, socket_work_thread, NULL);

    while(1) {
        usleep(100 * 1000);
        auto list = mysqli_query(mysql, "SELECT * FROM submission WHERE judged = false");
        for (int i = 0; i < list.size(); i++) {
            if (judging.find(atoi(list[i]["id"].c_str())) != judging.end()) continue;
            // 找空位
            while(1) {
                bool added = false;
                for (int j = 1; j <= judge["thread"].asInt(); j++) if (!isJudging[j]) {
                    added = true;
                    isJudging[j] = true;
                    judging.insert(atoi(list[i]["id"].c_str()));
                    std::ofstream fout("../tmp/" + std::to_string(j) + "/" + judge["languages"][atoi(list[i]["lang"].c_str())]["source_path"].asString());
                    fout << list[i]["code"];
                    fout.close();
                    Json::Value data = json_decode(readFile("../problem/" + list[i]["pid"] + "/config.json"));
                    if (data["spj"]["type"] == 0) {
                        fout.open("../tmp/" + std::to_string(j) + "/" + data["spj"]["source"].asString());
                        fout << readFile("../problem/" + list[i]["pid"] + "/" + data["spj"]["source"].asString());
                        fout.close();
                        fout.open("../tmp/" + std::to_string(j) + "/testlib.h");
                        fout << readFile("../spj/testlib.h");
                        fout.close();
                    }
                    writeLog(LOG_LEVEL_INFO, "Judge submission #%d in ../tmp/%d...", atoi(list[i]["id"].c_str()), j);
                    writeLog(LOG_LEVEL_INFO, "Problem id: %d", atoi(list[i]["pid"].c_str()));
                    writeLog(LOG_LEVEL_INFO, "Contest id: %d", atoi(list[i]["contest"].c_str()));
                    writeLog(LOG_LEVEL_INFO, "Submitted user id: %d", atoi(list[i]["uid"].c_str()));
                    writeLog(LOG_LEVEL_INFO, "Language: %s", judge["languages"][atoi(list[i]["lang"].c_str())]["name"].asCString());
                    auto contest = mysqli_query(
                        mysql,
                        "SELECT starttime, duration FROM contest WHERE id = %d",
                        atoi(list[i]["contest"].c_str())
                    );
                    pid_t pt; 
                    Args *arg = new Args;
                    *arg = { 
                        j, 
                        atoi(list[i]["id"].c_str()), 
                        atoi(list[i]["contest"].c_str()), 
                        atoi(list[i]["uid"].c_str()),
                        atoi(list[i]["pid"].c_str()),
                        "", 
                        NULL
                    };
                    arg->cmd = "./unit ../../judge/judge.json ../../problem/" + list[i]["pid"] + " ../tmp/" + std::to_string(j) + " ./server.sock " + list[i]["lang"] + " >> ./log.txt 2>&1";
                    arg->mysql = quick_mysqli_connect();
                    proc_create(&pt, work_thread, (void*)arg);
                    break;
                }
                if (added) break;
            }
        }
    };
}