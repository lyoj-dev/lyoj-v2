#include "../../../httpd.h"
#include "../../../utils.cpp"

auto ContestsProblemsListAll = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    bool signup = checkSignUp(userId, atoi(argv[0].c_str()));
    
    auto contest = mysqli_query(
        mysql,
        "SELECT problems, starttime, duration, uid FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    std::string where = "id in (" + contest[0]["problems"].substr(1, contest[0]["problems"].size() - 2) + ")";
    auto res = mysqli_query(
        mysql, 
        "SELECT id, title FROM problem WHERE %s",
        where.c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["items"].resize(0);
    if (identity != Administrator) {
        if (time(NULL) < atol(contest[0]["starttime"].c_str())) res = {};
        else if (time(NULL) < atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str())) {
            if (identity == Visitor) res = {};
        }
    }
    Json::Value problems = json_decode(contest[0]["problems"]);
    for (int i = 0; i < problems.size(); i++) {
        for (int j = 0; j < res.size(); j++) {
            if (res[j]["id"] == problems[i].asString()) {
                Json::Value single;
                single["id"] = i + 1;
                single["realId"] = atoi(res[j]["id"].c_str());
                single["title"] = res[j]["title"];
                object["items"].append(single);
                break;
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