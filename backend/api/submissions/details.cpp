#pragma once

#include "../../httpd.h"
#include "../../utils.cpp"
#include "../../../shared/socket.h"

auto SubmissionsDetails = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    bool isAdmin = hasPermission(userInfo, AdminPage);

    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM submission WHERE id = %d AND contest = %d", 
        atoi(argv[0].c_str()),
        argv.size() == 1 ? 0 : atoi(argv[1].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);
    auto problems = mysqli_query(
        mysql,
        "SELECT id, title, groups FROM problem WHERE id = %d",
        atoi(res[0]["pid"].c_str())
    );
    if (problems.size() == 0) quickSendMsg(404);
    if (argv.size() == 1 && !hasIntersection(json_decode(problems[0]["groups"]), userInfo["groups"])) quickSendMsg(403);
    auto users = mysqli_query(
        mysql,
        "SELECT id, title FROM user WHERE id = %d",
        atoi(res[0]["uid"].c_str())
    );
    if (users.size() == 0) quickSendMsg(404);
    auto contest = mysqli_query(
        mysql,
        "SELECT type, starttime, duration FROM contest WHERE id = %d",
        atoi(res[0]["contest"].c_str())
    );
    bool ended = contest.size() == 0 || time(NULL) > atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str());
    bool hidden = contest.size() && atoi(contest[0]["type"].c_str()) == 0;

    Json::Value fetchData;
    fetchData["type"] = DetailedSubmission;
    fetchData["item"] = atoi(argv[0].c_str());
    Connection client = Client(appConfig["judge.sock"].asString()).connect();
    client.send(json_encode(fetchData));
    Json::Value status = json_decode(client.recv());

    status["id"] = atoi(res[0]["id"].c_str());
    status["pid"] = atoi(res[0]["pid"].c_str());
    status["uid"] = atoi(res[0]["uid"].c_str());
    status["code"] = res[0]["code"];
    status["date"] = atoi(res[0]["time"].c_str());
    status["problem"] = problems[0]["title"];
    status["user"] = users[0]["title"];
    status["judged"] = atoi(res[0]["judged"].c_str());
    if (atoi(res[0]["uid"].c_str()) != userId && !hasPermission(userInfo, SubmissionCodeOthers)) status["code"] = "";
    if (atoi(res[0]["uid"].c_str()) == userId && !hasPermission(userInfo, SubmissionRejudge)) status["rejudge"] = false;
    else if (atoi(res[0]["uid"].c_str()) != userId && !hasPermission(userInfo, SubmissionRejudgeOthers)) status["rejudge"] = false;
    else status["rejudge"] = true;
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    if (argv.size() == 2) {
        object["identity"] = getContestIdentity(userId, atoi(argv[1].c_str()));
        object["signup"] = checkSignUp(userId, atoi(argv[1].c_str()));
    }
    object["item"] = status;

    // 对特定权限进行数据修改
    if (!hasPermission(userInfo, SubmissionData)) {
        for (int i = 0; i < object["item"]["subtasks"].size(); i++) {
            for (int j = 0; j < object["item"]["subtasks"][i]["datas"].size(); j++) {
                object["item"]["subtasks"][i]["datas"][j]["output"] = "";
                object["item"]["subtasks"][i]["datas"][j]["outputIgnored"] = 0;
            }
        }
    }
    
    // 对特定比赛进行数据修改
    if (hidden && !ended) {
        if (
            object["item"]["statusType"].asInt() != Waiting && 
            object["item"]["statusType"].asInt() != Compiling &&
            object["item"]["statusType"].asInt() != CE
        ) {
            object["item"]["statusType"] = Submitted;
            object["item"]["status"] = JudgeResultInfo[Submitted];
            object["item"]["judged"] = true;
            object["item"]["score"] = 100;
            object["item"]["memory"] = 0;
            object["item"]["time"] = 0;
            object["item"]["subtasks"].resize(0);
        }
    } else if (!ended) {
        for (int i = 0; i < object["item"]["subtasks"].size(); i++) {
            for (int j = 0; j < object["item"]["subtasks"][i]["datas"].size(); j++) {
                object["item"]["subtasks"][i]["datas"][j]["output"] = "";
                object["item"]["subtasks"][i]["datas"][j]["outputIgnored"] = 0;
            }
        }
    }

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};