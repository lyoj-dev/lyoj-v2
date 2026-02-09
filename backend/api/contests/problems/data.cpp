#include "../../../httpd.h"
#include "../../../utils.cpp"
#include "../../problems/data.cpp"

auto ContestsProblemsData = [](client_conn conn, http_request request, param argv) {
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
    int hidden = 0;
    if (identity != Administrator) {
        if (time(NULL) < atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str())) {
            hidden = 1;
        }
    }
    
    swap(argv[0], argv[1]);
    argv[1] = std::to_string(hidden);
    ProblemsData(conn, request, argv);
};