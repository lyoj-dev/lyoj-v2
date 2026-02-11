#include "../../../httpd.h"
#include "../../submissions/list.cpp"

auto AdminSubmissionList = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    argv.push_back("-1");
    SubmissionsList(conn, request, argv);
};