auto UsersUpdatePassword = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (userId != atoi(argv[0].c_str()) && !hasPermission(userInfo, UserEditOthers)) quickSendMsgWithoutMySQL(403);

    auto mysql = quick_mysqli_connect();
    auto data = json_decode(request.postdata);
    auto res = mysqli_query(mysql, "SELECT password FROM user WHERE id = %d", userId);
    if (res.size() == 0) quickSendMsg(404);
    if (res[0]["password"] != "" && res[0]["password"] != data["oldPassword"].asString()) quickSendMsg(403);
    mysqli_execute(
        mysql,
        "UPDATE user SET password = \"%s\" WHERE id = %d",
        data["newPassword"].asString().c_str(),
        userId
    );

    quickSendMsg(200);
};