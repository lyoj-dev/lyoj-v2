#include "../../../httpd.h"
#include "../../../utils.cpp"
#include "../../problems/submit.cpp"

auto ContestsProblemsSubmit = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    auto contest = mysqli_query(
        mysql,
        "SELECT uid, starttime, duration FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    if (identity != Administrator) {
        if (
            time(NULL) < atol(contest[0]["starttime"].c_str()) || 
            time(NULL) > atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str())
        ) { quickSendMsg(403); }
        else if (time(NULL) < atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str())) {
            if (identity == Visitor) quickSendMsg(403);
        }
    }
    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM contest_signup WHERE id = %d AND uid = %d", 
        atoi(argv[0].c_str()),
        userId
    );
    if (res.size() == 0 && userId != atoi(contest[0]["uid"].c_str())) quickSendMsg(403);
    mysqli_close(mysql);

    swap(argv[0], argv[1]);
    ProblemsSubmit(conn, request, argv);
};