#include "../../httpd.h"
#include "../../utils.cpp"

auto ContestsUnregister = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (userId == 0) quickSendMsgWithoutMySQL(401);

    MYSQL mysql = quick_mysqli_connect();
    auto contest = mysqli_query(
        mysql,
        "SELECT * FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    if (atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str()) < time(NULL)) quickSendMsg(403);
    auto res = mysqli_query(
        mysql,
        "SELECT COUNT(*) as count FROM contest_signup WHERE id = %d AND uid = %d",
        atoi(argv[0].c_str()),
        userId
    );
    if (atoi(res[0]["count"].c_str()) == 0) quickSendMsg(403);
    mysqli_execute(
        mysql,
        "DELETE FROM contest_signup WHERE id = %d AND uid = %d",
        atoi(argv[0].c_str()),
        userId
    );
    mysqli_execute(
        mysql,
        "DELETE FROM contest_ranking WHERE id = %d AND uid = %d",
        atoi(argv[0].c_str()),
        userId
    );
    mysqli_execute(
        mysql,
        "UPDATE submission SET contest = -1 WHERE uid = %d AND contest = %d",
        userId,
        atoi(argv[0].c_str())
    );
    mysqli_close(mysql);

    quickSendMsg(200);
};