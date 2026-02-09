#pragma once

#include <jsoncpp/json/value.h>
#include <mysql/mysql.h>
#include "../shared/mysql.h"
#include "../shared/json.h"
#include "../shared/type.h"
#include "httpd.h"

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
#define quickSendMsgWithoutMySQL(code) { \
    Json::Value msg; \
    msg["code"] = code; \
    msg["msg"] = http_code[code]; \
    msg["loginAs"] = userId; \
    msg["loginInfo"] = userInfo; \
    std::string responseBody = json_encode(msg); \
    auto response = __api_default_response; \
    response["Access-Control-Allow-Origin"] = request.argv["origin"]; \
    response["Content-Length"] = std::to_string(responseBody.size()); \
    putRequest(conn, 200, response); \
    send(conn, responseBody); \
    exitRequest(conn); \
}
#define quickSendMsg(code) { \
    mysqli_close(mysql); \
    quickSendMsgWithoutMySQL(code); \
}

bool hasPermission(Json::Value userInfo, int permission) {
    return (userInfo["permission"].asInt() & permission) == permission;
}
int getUserId(http_request request) {
    std::string authorization = request.argv["authorization"];
    if (authorization.substr(0, 13) != "SessionToken ") return 0;
    std::string sessionToken = authorization.substr(13);
    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(
        mysql,
        "SELECT uid FROM session WHERE session = \"%s\" AND time > %ld",
        sessionToken.c_str(),
        time(NULL) - 30 * 24 * 60 * 60
    );
    mysqli_close(mysql);
    if (res.size() == 0) return 0;
    return atoi(res[0]["uid"].c_str());
}
Json::Value getUserInfo(int id) {
    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(mysql, "SELECT * FROM user WHERE id = %d", id);
    if (res.size() == 0) return mysqli_close(mysql), Json::objectValue;
    Json::Value obj = Json::objectValue;
    obj["title"] = res[0]["title"];
    obj["groups"] = json_decode(res[0]["groups"]);
    res = mysqli_query(
        mysql,
        "SELECT BIT_OR(permission) as permission FROM userGroup WHERE id in (%s)", 
        res[0]["groups"].substr(1, res[0]["groups"].size() - 2).c_str()
    );
    if (res.size() == 0) return mysqli_close(mysql), Json::objectValue;
    obj["permission"] = atoi(res[0]["permission"].c_str());
    mysqli_close(mysql);
    return obj;
}
ContestIdentity getContestIdentity(int uid, int cid) {
    Json::Value userInfo = getUserInfo(uid);
    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(mysql, "SELECT * FROM user WHERE id = %d", uid);
    if (res.size() == 0) return mysqli_close(mysql), Visitor;
    auto contest = mysqli_query(mysql, "SELECT uid FROM contest WHERE id = %d", cid);
    if (contest.size() == 0) return mysqli_close(mysql), Visitor;
    if (uid == atoi(contest[0]["uid"].c_str()) && hasPermission(userInfo, ContestEdit)) return mysqli_close(mysql), Administrator;
    if (uid != atoi(contest[0]["uid"].c_str()) && hasPermission(userInfo, ContestEditOthers)) return mysqli_close(mysql), Administrator;
    res = mysqli_query(mysql, "SELECT COUNT(*) AS count FROM contest_signup WHERE id = %d AND uid = %d", cid, uid);
    mysqli_close(mysql);
    return atoi(res[0]["count"].c_str()) ? Contestant : Visitor;
}
bool checkSignUp(int uid, int cid) {
    if (uid == 0) return false;
    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(mysql, "SELECT COUNT(*) as count FROM contest_signup WHERE id = %d AND uid = %d", cid, uid);
    mysqli_close(mysql);
    return atoi(res[0]["count"].c_str());
}
bool hasIntersection(Json::Value a, Json::Value b) {
    bool flag = false;
    for (int i = 0; i < a.size(); i++)
        for (int j = 0; j < b.size(); j++) if (a[i] == b[j]) return true;
    return false;
}
std::string hasIntersection(std::string key, Json::Value goal, bool ifEmptyValue = true) {
    std::string where = "REPLACE(REPLACE(" + key + ", ']', ','), '[', ',') REGEXP '";
    for (int i = 0; i < goal.size(); i++) where += "," + goal[i].asString() + "," + (i + 1 < goal.size() ? "|" : "");
    where += "'";
    if (goal.size() == 0 && !ifEmptyValue) where = "0 "; // 特判没有值的情况
    return where;
}