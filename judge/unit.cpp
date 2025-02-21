/**
 * @file unit.cpp
 * @author LittleYang0531 (littleyang0531@cau.edu.cn)
 * @brief 评测机服务核心单元文件
 * @version 1.0.0
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2025 LittleYang0531
 * 
 */

#include<bits/stdc++.h>
#include<pthread.h>
#define __windows__ (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#if __linux__ 
#include<signal.h>
#include<sys/resource.h>
#include<sys/wait.h>
#include<sys/syscall.h>
#include<sys/un.h>
#include<unistd.h>
#include<arpa/inet.h>
// #elif __windows__
// #include<Windows.h>
// #include<direct.h>
// #include<winsock.h>
#endif

using namespace std;
string workPath = "", dataPath = "";
#include"../shared/json.h"
#include"../shared/log.h"
#include"../shared/type.h"
#include"../shared/socket.h"
#include"../shared/utils.h"
Json::Value judge;
#include"system.h"
#include"core.h"

int main(int argc, char** argv) {
    if (argc < 6) {
        cout << "LYOJ 评测机单元程序" << endl;
        cout << endl;
        cout << "Usage: " << argv[0] << " <judge.json> <data-path> <tmp-path> <server.sock> <lang>" << endl;
        cout << "    judge.json: 评测机配置文件" << endl;
        cout << "    data-path: 数据文件目录" << endl;
        cout << "    tmp-path: 临时文件目录" << endl;
        cout << "    server.sock: 数据服务套接字文件" << endl;
        cout << "    lang: 编程语言 id" << endl;
        cout << endl;
        cout << "注意: " << endl;
        cout << "    此程序不应由用户手动执行，" << endl;
        cout << "    应由评测机主程序创建评测任务后新建监听套接字，" << endl;
        cout << "    随后评测机主程序调用该程序并利用该套接字通信。" << endl;
        return 1;
    }
    // system("export PATH=$PATH:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/sbin");
    // writeLog(LOG_LEVEL_INFO, "%s", system2("echo $PATH").c_str());
    workPath = argv[3], dataPath = argv[2];
    __chdir(workPath);
    judge = json_decode(readFile(argv[1]));
    Json::Value data = json_decode(readFile(dataPath + "/config.json"));
    string serverSock = argv[4];
    int lang = atoi(argv[5]);
    Client client(serverSock);
    Connection conn = client.connect();
    
    map<int, int> subtasks = { { 0, 0 } };
    vector<vector<int> > subtasks_depends(data["subtasks"].size() + 1, vector<int>());
    vector<vector<int> > subtasks_data(data["subtasks"].size() + 1, vector<int>());
    int *depends = new int[data["subtasks"].size() + 1];
    memset(depends, 0, (data["subtasks"].size() + 1) * sizeof(int));
    for (int i = 0; i < data["subtasks"].size(); i++) subtasks[data["subtasks"][i]["id"].asInt()] = i + 1;
    for (int i = 0; i < data["datas"].size(); i++) subtasks_data[subtasks[data["datas"][i]["subtask"].asInt()]].push_back(i);
    for (int i = 0; i < data["subtasks"].size(); i++) for (int j = 0; j < data["subtasks"][i]["depends"].size(); j++) {
        int curr = subtasks[data["subtasks"][i]["id"].asInt()];
        int depend = subtasks[data["subtasks"][i]["depends"][j].asInt()];
        subtasks_depends[depend].push_back(curr);
        depends[curr]++;
    }

    // 构造结果
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
    conn.send(json_encode(res));

    // 编译源文件
    res["status"] = Compiling;
    conn.send(json_encode(res));
    time_t st = clock2();
    if (judge["languages"][lang]["type"].asInt() != 1) {
        writeLog(LOG_LEVEL_INFO, "Compiling source code...");
        string info = "";
        int ret = system2(judge["languages"][lang]["command"].asString() + " 2>&1", info);
        if (ret) {
            writeLog(LOG_LEVEL_INFO, "Failed to compile source code!");
            writeLog(LOG_LEVEL_INFO, "Compiler returned %d", ret);
            res["status"] = CE;
            res["info"] = info;
            conn.send(json_encode(res));
            return 0;
        }
        writeLog(LOG_LEVEL_INFO, "Compile finished, used %dms.", (clock2() - st));
        res["info"] = info;
    }

    // 编译 SPJ
    st = clock2();
    if (data["spj"]["type"].asInt() == 0) {
        writeLog(LOG_LEVEL_INFO, "Compiling special judge..");
        string info = "";
        int ret = system2(data["spj"]["compile_cmd"].asString() + " 2>&1", info);
        if (ret) {
            writeLog(LOG_LEVEL_INFO, "Failed to compile special judge!");
            writeLog(LOG_LEVEL_INFO, "Compiler returned %d", ret);
            res["status"] = CE;
            res["info"] = res["info"].asString() + "In SPJ:\n" + info;
            conn.send(json_encode(res));
            return 0;
        }
        writeLog(LOG_LEVEL_INFO, "Compile finished, used %dms.", (clock2() - st));
        if (info != "") res["info"] = res["info"].asString() + "In SPJ:\n" + info;
    } else {
        system2("cp \"" + judge["spj"][data["spj"]["type"].asInt() - 1]["path"].asString() + "\" ./spj");
    }

    // 构造子任务结果
    for (int i = 0; i < data["subtasks"].size() + 1; i++) {
        Json::Value subtask;
        subtask["id"] = i;
        subtask["title"] = i ? data["subtasks"][i - 1]["title"] : "";
        subtask["status"] = Waiting;
        subtask["score"] = 0;
        subtask["datas"].resize(0);
        for (int j = 0; j < subtasks_data[i].size(); j++) {
            Json::Value data;
            data["status"] = Waiting;
            data["score"] = 0;
            data["time"] = 0;
            data["memory"] = 0;
            data["info"] = "";
            data["output"] = "";
            data["outputIgnored"] = 0;
            subtask["datas"].append(data);
        }
        res["subtasks"].append(subtask);
    }

    res["status"] = Running;
    conn.send(json_encode(res));

    // 拓扑排序
    int sum_t = 0, max_m = 0;
    queue<int> q;
    for (int i = 0; i < data["subtasks"].size() + 1; i++) if (depends[i] == 0) q.push(i);
    int submitResult = -1, cnt = 0;
    while (!q.empty()) {
        int curr = q.front(); q.pop();
        bool accepted = true;
        Json::Value &subtask = res["subtasks"][curr];
        subtask["status"] = Running;
        int subtaskResult = -1;
        Json::Value dat = Json::objectValue;
        dat["status"] = Running;
        dat["score"] = 0;
        dat["type"] = UpdateSubtask;
        dat["subtaskId"] = curr;
        conn.send(json_encode(dat));
        for (int i = 0; i < subtasks_data[curr].size(); i++) {
            Json::Value dat = Json::objectValue;
            dat["status"] = Running;
            dat["score"] = 0;
            dat["time"] = 0;
            dat["memory"] = 0;
            dat["info"] = "";
            dat["type"] = UpdateData;
            dat["subtaskId"] = curr;
            dat["dataId"] = i;
            dat["output"] = "";
            dat["outputIgnored"] = 0;
            conn.send(json_encode(dat));
            Json::Value &result = subtask["datas"][i];
            if (curr && !accepted) {
                result["status"] = Skipped;
                result["info"] = "Skipped";
                Json::Value dat = result;
                dat["type"] = UpdateData;
                dat["subtaskId"] = curr;
                dat["dataId"] = i;
                conn.send(json_encode(dat));
                continue;
            }
            time_t st = clock2();
            int now_status, t, m;
            writeLog(LOG_LEVEL_INFO, "Running test case %d-%d...", curr, i + 1);
            Json::Value status = judge_data(subtasks_data[curr][i], lang, now_status, t, m, judge, data);
            sum_t += t, max_m = max(max_m, m);
            if (subtaskResult == -1 && now_status != AC) subtaskResult = now_status;
            if (submitResult == -1 && now_status != AC) submitResult = now_status;
            if (now_status != AC) accepted = false;
            result["status"] = now_status;
            result["score"] = status["score"];
            result["time"] = t;
            result["memory"] = m;
            result["info"] = status["info"];
            result["output"] = status["output"];
            result["outputIgnored"] = status["outputIgnored"];
            dat = result;
            dat["type"] = UpdateData;
            dat["subtaskId"] = curr;
            dat["dataId"] = i;
            conn.send(json_encode(dat));
        }
        // 计算分数
        int score = 0;
        if (curr) switch (data["subtasks"][curr - 1]["type"].asInt()) {
            case 0: case 3: 
                for (int i = 0; i < subtasks_data[curr].size(); i++) 
                    score += subtask["datas"][i]["score"].asInt(); 
                break;
            case 1: 
                for (int i = 0; i < subtasks_data[curr].size(); i++) 
                    score = max(score, subtask["datas"][i]["score"].asInt()); 
                break;
            case 2: 
                score = 1e9; 
                for (int i = 0; i < subtasks_data[curr].size(); i++) 
                    score = min(score, subtask["datas"][i]["score"].asInt()); 
                break;
        }
        if (curr && data["subtasks"][curr - 1]["type"].asInt() == 3) score /= subtasks_data[curr].size();
        subtask["score"] = score;

        if (subtasks_data[curr].size() == 0) subtask["status"] = NTD;
        else if (subtaskResult == -1) subtask["status"] = AC;
        else subtask["status"] = subtaskResult;
        dat = subtask;
        dat["type"] = UpdateSubtask;
        dat["subtaskId"] = curr;
        dat.removeMember("datas");
        conn.send(json_encode(dat));

        if (subtaskResult == -1) for (int i = 0; i < subtasks_depends[curr].size(); i++) {
            depends[subtasks_depends[curr][i]]--;
            if (depends[subtasks_depends[curr][i]] == 0) q.push(subtasks_depends[curr][i]);
        }
    }

    for (int i = 0; i < data["subtasks"].size() + 1; i++) if (depends[i]) {
        Json::Value &subtask = res["subtasks"][i];
        subtask["status"] = Skipped;
        Json::Value dat = subtask;
        dat["type"] = UpdateSubtask;
        dat["subtaskId"] = i;
        dat.removeMember("datas");
        conn.send(json_encode(dat));
        for (int j = 0; j < subtasks_data[i].size(); j++) {
            Json::Value &result = subtask["datas"][j];
            result["status"] = Skipped;
            result["info"] = "Skipped";
            Json::Value dat = result;
            dat["type"] = UpdateData;
            dat["subtaskId"] = i;
            dat["dataId"] = j;
            conn.send(json_encode(dat));
        }
    }

    int score = 0;
    for (int i = 0; i < subtasks_data[0].size(); i++)
        score += res["subtasks"][0]["datas"][i]["score"].asInt();
    for (int i = 1; i < data["subtasks"].size() + 1; i++)
        score += res["subtasks"][i]["score"].asInt();

    res["status"] = submitResult == -1 ? AC : submitResult;
    res["score"] = score;
    res["time"] = sum_t;
    res["memory"] = max_m;

    if (data["datas"].size() == 0) res["status"] = NTD;
    Json::Value dat = res;
    dat["type"] = UpdateSubmission;
    dat.removeMember("subtasks");
    conn.send(json_encode(dat));
}