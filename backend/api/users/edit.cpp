#include "../../httpd.h"
#include "../../utils.cpp"

auto UsersEdit = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (userId != atoi(argv[0].c_str()) && !hasPermission(userInfo, UserEditOthers)) quickSendMsgWithoutMySQL(403);

    auto mysql = quick_mysqli_connect();
    auto data = json_decode(request.postdata);
    std::string title = data["title"].asString();
    std::string info = data["info"].asString();
    if (atoi(mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count FROM user WHERE title = \"%s\"",
        title.c_str()
    )[0]["count"].c_str()) && title != userInfo["title"].asString()) quickSendMsg(409);

    mysqli_execute(
        mysql,
        "UPDATE user SET title = \"%s\", info = \"%s\" WHERE id = %d",
        title.c_str(),
        info.c_str(),
        atoi(argv[0].c_str())
    );

    quickSendMsg(200);
};