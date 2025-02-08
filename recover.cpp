#include<bits/stdc++.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;
#include"./shared/type.h"
#include"./shared/log.h"
#include"./shared/mysql.h"
#include"./shared/utils.h"
#include"./shared/json.h"
map<string, int> statusIdMap;
int main() {
    Json::Value judge = json_decode(readFile("./judge/judge.json"));
    MYSQL mysql2 = mysqli_connect("127.0.0.1", "", "", "lyoj", 3306);
    auto problems = mysqli_query(mysql2, "SELECT * FROM problem ORDER BY id ASC");
    auto tags = mysqli_query(mysql2, "SELECT * FROM tags WHERE type = \"problem\"");
    auto tagsGrouped = mysqli_query(mysql2, "SELECT * FROM tags GROUP BY tagname");
    auto submissions = mysqli_query(mysql2, "SELECT * FROM status ORDER BY id ASC");
    MYSQL mysql = mysqli_connect("127.0.0.1", "", "", "judge", 3306);
    int tagId = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM tags")[0]["count"].c_str());
    int problemId = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM problem")[0]["count"].c_str()) + 1;
    int submissionId = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM submission")[0]["count"].c_str()) + 1;
    map<string, int> tagIdMap;
    map<int, int> problemIdMap;
    cout << "Recovering Tags..." << endl;
    for (int i = 0; i < tagsGrouped.size(); i++) {
        mysqli_execute(
            mysql,
            "INSERT INTO tags VALUES (%d, \"%s\", %d)",
            tagId,
            quote_encode(tagsGrouped[i]["tagname"]).c_str(),
            0
        );
        cout << "Added Tag \"" << tagsGrouped[i]["tagname"] << "\"!" << endl;
        tagIdMap[tagsGrouped[i]["tagname"].c_str()] = tagId;
        tagId++;
    }
    cout << "Recovering Problems..." << endl;
    for (int i = 0; i < problems.size(); i++) {
        string tagsList = "";
        for (int j = 0; j < tags.size(); j++) if (tags[j]["id"] == problems[i]["id"]) {
            tagsList += (tagsList == "" ? "" : ",") + to_string(tagIdMap[tags[j]["tagname"]]);
        }
        tagsList = "[0," + tagsList + "]";
        mysqli_execute(
            mysql,
            "INSERT INTO problem VALUES (%d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", 0, 0, %d, \"%s\", 0)",
            problemId,
            string("P" + problems[i]["pid"]).c_str(),
            quote_encode(problems[i]["name"]).c_str(),
            quote_encode(problems[i]["bg"]).c_str(),
            quote_encode(problems[i]["descrip"]).c_str(),
            quote_encode(problems[i]["input"]).c_str(),
            quote_encode(problems[i]["output"]).c_str(),
            quote_encode(problems[i]["cases"]).c_str(),
            quote_encode(problems[i]["hint"]).c_str(),
            atoi(problems[i]["difficult"].c_str()),
            tagsList.c_str()
        );
        problemIdMap[atoi(problems[i]["id"].c_str())] = problemId;
        problemId++;
        Json::Value data = json_decode(readFile("./old_problem/" + problems[i]["id"] + "/config.json"));
        data["datas"] = data["data"]; data.removeMember("data");
        int subtaskNumber = 0;
        for (int j = 0; j < data["datas"].size(); j++) subtaskNumber = max(subtaskNumber, data["datas"][j]["subtask"].asInt());
        for (int j = 0; j < subtaskNumber; j++) {
            Json::Value subtask;
            subtask["id"] = j + 1;
            subtask["title"] = "Subtask #" + to_string(j + 1);
            subtask["type"] = 2;
            subtask["depends"] = data["subtask_depend"][j];
            data["subtasks"].append(subtask);
        }
        data.removeMember("subtask_depend");
        mkdir(("./problem/" + problems[i]["id"]).c_str(), 0777);
        ofstream fout("./problem/" + problems[i]["id"] + "/config.json");
        fout << json_encode(data);
        fout.close();
        fout.open("./problem/" + problems[i]["id"] + "/problem.md");
        if (problems[i]["bg"] != "") fout << "# 题目背景" << endl << endl << problems[i]["bg"] << endl << endl;
        if (problems[i]["descrip"] != "") fout << "# 题目描述" << endl << endl << problems[i]["descrip"] << endl << endl;
        if (problems[i]["input"] != "") fout << "# 输入格式" << endl << endl << problems[i]["input"] << endl << endl;
        if (problems[i]["output"] != "") fout << "# 输出格式" << endl << endl << problems[i]["output"] << endl << endl;
        if (problems[i]["cases"] != "") {
            Json::Value cases = json_decode(problems[i]["cases"]);
            fout << "# 样例" << endl << endl;
            for (int j = 0; j < cases.size(); j++) {
                fout << "## 样例输入 " << j + 1 << endl << endl;
                fout << "```" << endl << cases[j]["input"].asString() << endl << "```" << endl << endl;
                fout << "## 样例输出 " << j + 1 << endl << endl;
                fout << "```" << endl << cases[j]["output"].asString() << endl << "```" << endl << endl;
            }
        }
        if (problems[i]["hint"] != "") fout << "# 数据范围与提示" << endl << endl << problems[i]["hint"] << endl;
        fout.close();
        Json::Value meta;
        meta["id"] = problemId - 1;
        meta["alias"] = "P" + problems[i]["pid"];
        meta["title"] = problems[i]["name"];
        meta["difficulty"] = atoi(problems[i]["difficult"].c_str());
        meta["tags"].resize(0);
        for (int j = 0; j < tags.size(); j++) if (tags[j]["id"] == problems[i]["id"]) meta["tags"].append(tags[j]["tagname"]);
        fout.open("./problem/" + problems[i]["id"] + "/meta.json");
        fout << json_encode(meta);
        fout.close();
        cout << "Added Problem \"" << problems[i]["name"] << "\"(id = " << problems[i]["id"] << ")!" << endl;
    }
    for (int i = AC; i <= Submitted; i++) statusIdMap[JudgeResultInfo[i]] = i;
    cout << "Recovering Submissions..." << endl;
    for (int i = 0; i < submissions.size(); i++) {
        Json::Value result = json_decode(submissions[i]["result"]);
        Json::Value data = json_decode(readFile("./problem/" + submissions[i]["pid"] + "/config.json"));
        result["status"] = statusIdMap[submissions[i]["status"]];
        result["score"] = atoi(submissions[i]["score"].c_str());
        result["langCode"] = atoi(submissions[i]["lang"].c_str());
        result["langMode"] = judge["languages"][atoi(submissions[i]["lang"].c_str())]["mode"];
        result["langName"] = judge["languages"][atoi(submissions[i]["lang"].c_str())]["name"];
        int maxSubtaskId = 0;
        for (int j = 0; j < result["info"].size(); j++) maxSubtaskId = max(maxSubtaskId, data["datas"][j]["subtask"].asInt());
        result["subtasks"].resize(maxSubtaskId + 1);
        for (int j = 0; j < result["info"].size(); j++) {
            string info = result["info"][j]["info"].asString();
            while (info.size() && info.back() == '\n') info.pop_back();
            result["info"][j]["info"] = info;
            result["info"][j]["status"] = statusIdMap[result["info"][j]["state"].asString()];
            result["info"][j]["output"] = "";
            result["info"][j]["outputIgnored"] = 0;
            result["info"][j].removeMember("state");
            result["subtasks"][data["datas"][j]["subtask"].asInt()]["datas"].append(result["info"][j]);
        }
        for (int j = 0; j <= maxSubtaskId; j++) {
            int status = AC, score = 1e9;
            result["subtasks"][j]["title"] = "Subtask #" + to_string(j);
            for (int k = 0; k < result["subtasks"][j]["datas"].size(); k++) {
                if (result["subtasks"][j]["datas"][k]["status"].asInt() != AC && status == AC) status = result["subtasks"][j]["datas"][k]["status"].asInt();
                score = min(score, result["subtasks"][j]["datas"][k]["score"].asInt());
            }
            result["subtasks"][j]["id"] = j;
            result["subtasks"][j]["status"] = status;
            result["subtasks"][j]["score"] = score;
        }
        result["info"] = result["compile_info"];
        result["subtasks"][0]["title"] = "";
        result["subtasks"][0]["status"] = result["status"];
        result["subtasks"][0]["score"] = result["score"];
        result.removeMember("state");
        result.removeMember("compile_info");
        mysqli_execute(
            mysql,
            "INSERT INTO submission VALUES (%d, %d, %d, %d, \"%s\", \"%s\", %d, %d, %d, %d, %d)",
            submissionId++, 
            atoi(submissions[i]["uid"].c_str()),
            problemIdMap[atoi(submissions[i]["pid"].c_str())],
            atoi(submissions[i]["lang"].c_str()),
            quote_encode(submissions[i]["code"]).c_str(),
            quote_encode(json_encode(result)).c_str(),
            atoi(submissions[i]["time"].c_str()),
            statusIdMap[submissions[i]["status"]],
            atoi(submissions[i]["score"].c_str()),
            atoi(submissions[i]["judged"].c_str()),
            atoi(submissions[i]["contest"].c_str())
        );
        cout << "Added Submission #" << submissions[i]["id"] << "!" << endl;
    }
}