#pragma once

#include "../../httpd.h"
#include "../../utils.cpp"

auto ProblemsSubmit = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (userId == 0) quickSendMsgWithoutMySQL(401);

    auto $_POST = json_decode(request.postdata);
    MYSQL mysql = quick_mysqli_connect();

    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM problem WHERE id = " + argv[0])[0]["count"].c_str());
    if (!exist) quickSendMsg(404);

    // 权限检查
    auto res = mysqli_query(mysql, "SELECT langs, groups FROM problem WHERE id = " + argv[0]);
    if (argv.size() == 1 && !hasIntersection(json_decode(res[0]["groups"]), userInfo["groups"])) quickSendMsg(403);
    Json::Value langs; langs.append($_POST["lang"].asInt());
    if (!hasIntersection(json_decode(res[0]["langs"]), langs)) quickSendMsg(403);

    int id = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM submission")[0]["count"].c_str()) + 1;
    int uid = userId;
    int pid = atoi(argv[0].c_str());
    int lang = $_POST["lang"].asInt();
    std::string code = $_POST["code"].asString();
    std::string result = "{}";
    time_t time = std::time(NULL);
    int status = Waiting;
    int score = 0;
    bool judged = false;
    int contest = argv.size() == 2 ? atoi(argv[1].c_str()) : 0;

    mysqli_execute(
        mysql,
        "INSERT INTO submission VALUES (%d, %d, %d, %d, \"%s\", \"%s\", %ld, %d, %d, %d, %d)",
        id, uid, pid, lang, quote_encode(code).c_str(), result.c_str(), time, status, score, judged, contest
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["id"] = id;

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};