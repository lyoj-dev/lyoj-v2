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
    auto problems = mysqli_query(mysql2, "SELECT * FROM problem ORDER BY pid ASC");
    auto tags = mysqli_query(mysql2, "SELECT * FROM tags WHERE type = \"problem\"");
    auto tagsGrouped = mysqli_query(mysql2, "SELECT * FROM tags GROUP BY tagname");
    auto submissions = mysqli_query(mysql2, "SELECT * FROM status ORDER BY id ASC");
    MYSQL mysql = mysqli_connect("127.0.0.1", "", "", "judge", 3306);
    int tagId = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM tags")[0]["count"].c_str());
    int problemId = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM problem")[0]["count"].c_str()) + 1;
    int submissionId = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM submission")[0]["count"].c_str()) + 1;
    map<string, int> tagIdMap;
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
            problemId++,
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
        cout << "Added Problem \"" << problems[i]["name"] << "\"(id = " << problems[i]["id"] << ")!" << endl;
    }
    for (int i = AC; i <= Submitted; i++) statusIdMap[JudgeResultInfo[i]] = i;
    cout << "Recovering Submissions..." << endl;
    for (int i = 0; i < submissions.size(); i++) {
        Json::Value result = json_decode(submissions[i]["result"]);
        result["status"] = statusIdMap[submissions[i]["status"]];
        result["score"] = atoi(submissions[i]["score"].c_str());
        result["langCode"] = atoi(submissions[i]["lang"].c_str());
        result["langMode"] = judge["languages"][atoi(submissions[i]["lang"].c_str())]["mode"];
        result["langName"] = judge["languages"][atoi(submissions[i]["lang"].c_str())]["name"];
        result["subtasks"][0]["datas"] = result["info"];
        result["info"] = result["compile_info"];
        for (int j = 0; j < result["subtasks"][0]["datas"].size(); j++) {
            result["subtasks"][0]["datas"][j]["status"] = statusIdMap[result["subtasks"][0]["datas"][j]["state"].asString()];
            result["subtasks"][0]["datas"][j]["output"] = "";
            result["subtasks"][0]["datas"][j]["outputIgnored"] = 0;
            result["subtasks"][0]["datas"][j].removeMember("state");
        }
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
            atoi(submissions[i]["pid"].c_str()),
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