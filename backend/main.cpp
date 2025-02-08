/**
 * @file main.cpp
 * @author LittleYang0531 (littleyang0531@cau.edu.cn)
 * @brief 后端服务器主文件
 * @version 1.0.0
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2025 LittleYang0531
 * 
 */

#include<bits/stdc++.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;
#include"../shared/json.h"
#include"../shared/log.h"
#include"../shared/mysql.h"
#include"../shared/socket.h"
#include"../shared/utils.h"
#include"../shared/type.h"
#include"encrypt.h"
#include"httpd.h"

Json::Value appConfig;
MYSQL quick_mysqli_connect() {
    return mysqli_connect(
        appConfig["mysql"]["server"].asString(),
        appConfig["mysql"]["user"].asString(),
        appConfig["mysql"]["passwd"].asString(),
        appConfig["mysql"]["database"].asString(),
        appConfig["mysql"]["port"].asInt()
    );
}
#define quickSendMsg(code) { \
    Json::Value msg; \
    msg["code"] = code; \
    msg["msg"] = http_code[code]; \
    msg["loginAs"] = userId; \
    msg["loginInfo"] = userInfo; \
    string responseBody = json_encode(msg); \
    auto response = __api_default_response; \
    response["Access-Control-Allow-Origin"] = request.argv["origin"]; \
    response["Content-Length"] = to_string(responseBody.size()); \
    putRequest(conn, 200, response); \
    send(conn, responseBody); \
    exitRequest(conn); \
}
int getUserId(http_request request) {
    string authorization = request.argv["authorization"];
    if (authorization.substr(0, 13) != "SessionToken ") return 0;
    string sessionToken = authorization.substr(13);
    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(
        mysql,
        "SELECT uid FROM session WHERE session = \"%s\" AND time > %lld",
        sessionToken.c_str(),
        time(NULL) - 30 * 24 * 60 * 60
    );
    mysqli_close(mysql);
    if (res.size() == 0) return 0;
    return atoi(res[0]["uid"].c_str());
}
Json::Value getUserInfo(int id) {
    if (id == 0) return Json::objectValue;
    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(mysql, "SELECT * FROM user WHERE id != 0 AND id = %d", id);
    if (res.size() == 0) return Json::objectValue;
    Json::Value obj = Json::objectValue;
    obj["title"] = res[0]["title"];
    obj["groups"] = json_decode(res[0]["groups"]);
    res = mysqli_query(
        mysql,
        "SELECT BIT_OR(permission) as permission FROM userGroup WHERE id in (%s)", 
        res[0]["groups"].substr(1, res[0]["groups"].size() - 2).c_str()
    );
    if (res.size() == 0) return Json::objectValue;
    obj["permission"] = atoi(res[0]["permission"].c_str());
    mysqli_close(mysql);
    return obj;
}
bool hasIntersection(Json::Value a, Json::Value b) {
    bool flag = false;
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < b.size(); j++) if (a[i] == b[j]) return true;
    return false;
}
string hasIntersection(string key, Json::Value goal, bool ifEmptyValue = true) {
    string where = "REPLACE(REPLACE(groups, ']', ','), '[', ',') REGEXP '";
    for (int i = 0; i < goal.size(); i++) where += "," + goal[i].asString() + "," + (i + 1 < goal.size() ? "|" : "");
    where += "'";
    if (goal.size() == 0 && !ifEmptyValue) where = "0 "; // 特判没有值的情况
    return where;
}

#include"api/configurations/languages.cpp"
#include"api/configurations/spjs.cpp"
#include"api/problems/list.cpp"
#include"api/problems/listAll.cpp"
#include"api/problems/listAllTags.cpp"
#include"api/problems/details.cpp"
#include"api/problems/delete.cpp"
#include"api/problems/submit.cpp"
#include"api/problems/data.cpp"
#include"api/problems/config.cpp"
#include"api/submissions/list.cpp"
#include"api/submissions/details.cpp"
#include"api/users/login.cpp"
#include"api/users/check.cpp"
#include"api/users/listAll.cpp"
#include"api/users/listAllGroups.cpp"
#include"ws/submissions/list.cpp"
#include"ws/submissions/details.cpp"

void* judge_thread(void* argv) {
    system("cd ../judge && ./judge");
    return NULL;
}

int main() {
    pthread_t pt;
    // pthread_create(&pt, NULL, judge_thread, NULL);

    appConfig = json_decode(readFile("./config.json"));
    app.setopt(HTTP_ENABLE_SSL, appConfig["server.enableSSL"].asBool());
    app.setopt(HTTP_LISTEN_HOST, appConfig["server.listenHost"].asString().c_str());
    app.setopt(HTTP_LISTEN_PORT, appConfig["server.listenPort"].asInt());
    app.setopt(HTTP_SSL_CACERT, appConfig["server.httpsCacert"].asString().c_str());
    app.setopt(HTTP_SSL_PRIVKEY, appConfig["server.httpsPrivkey"].asString().c_str());
    app.setopt(HTTP_MULTI_THREAD, appConfig["server.threadNumber"].asInt());

    app.addRoute("/configurations/languages", ConfigurationsLanguages);
    app.addRoute("/configurations/spjs", ConfigurationsSPJs);
    app.addRoute("/problems/list", ProblemsList);
    app.addRoute("/problems/listAll", ProblemsListAll);
    app.addRoute("/problems/listAllTags", ProblemsListAllTags);
    app.addRoute("/problems/%d", ProblemsDetails);
    app.addRoute("/problems/%d/delete", ProblemsDelete);
    app.addRoute("/problems/%d/submit", ProblemsSubmit);
    app.addRoute("/problems/%d/data", ProblemsData);
    app.addRoute("/problems/%d/config", ProblemsConfig);
    // app.addRoute("/problems/%d/data/%s", ProblemsData);
    app.addRoute("/submissions/list", SubmissionsList);
    app.addRoute("/submissions/%d", SubmissionsDetails);
    app.addRoute("/users/login", UsersLogin);
    app.addRoute("/users/check", UsersCheck);
    app.addRoute("/users/listAll", UsersListAll);
    app.addRoute("/users/listAllGroups", UsersListAllGroups);

    app.ws_addRoute("/submissions/list", WSSubmissionsList);
    app.ws_addRoute("/submissions/%d", WSSubmissionsDetails);

    __default_response["Access-Control-Allow-Credentials"] = "true";
    __default_response["Access-Control-Allow-Headers"] = "*";
    __default_response["Access-Control-Allow-Methods"] = "*";
    app.run();
}