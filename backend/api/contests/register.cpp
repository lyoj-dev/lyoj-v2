#include "../../httpd.h"
#include "../../utils.cpp"

auto ContestsRegister = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (userId == 0) quickSendMsgWithoutMySQL(401);
    auto $_POST = json_decode(request.postdata);

    MYSQL mysql = quick_mysqli_connect();
    auto contest = mysqli_query(
        mysql,
        "SELECT * FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    if (atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str()) < time(NULL)) quickSendMsg(403);
    if (contest[0]["password"] != "" && contest[0]["password"] != $_POST["password"].asString()) quickSendMsg(403);
    auto res = mysqli_query(
        mysql,
        "SELECT COUNT(*) as count FROM contest_signup WHERE id = %d AND uid = %d",
        atoi(argv[0].c_str()),
        userId
    );
    if (atoi(res[0]["count"].c_str())) quickSendMsg(403);
    mysqli_execute(
        mysql,
        "INSERT INTO contest_signup VALUES (%d, %d)",
        atoi(argv[0].c_str()),
        userId
    );
    Json::Value info, problems = json_decode(contest[0]["problems"]);
    for (int i = 0; i < problems.size(); i++) {
        info[i]["pid"] = problems[i];
        info[i]["score"] = 0;
        info[i]["sid"] = 0;
        info[i]["time"] = 0;
        info[i]["time2"] = 0;
        info[i]["penalty"] = 0;
    }
    mysqli_execute(
        mysql,
        "INSERT INTO contest_ranking VALUES (%d, %d, 0, 0, 0, \"%s\", 0)",
        atoi(argv[0].c_str()),
        userId,
        quote_encode(json_encode(info)).c_str()
    );
    mysqli_close(mysql);

    quickSendMsg(200);
};