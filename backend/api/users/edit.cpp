auto UsersEdit = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (userId != atoi(argv[0].c_str()) && !hasPermission(userInfo, UserEditOthers)) quickSendMsgWithoutMySQL(403);

    auto mysql = quick_mysqli_connect();
    auto data = json_decode(request.postdata);
    string title = data["title"].asString();
    string info = data["info"].asString();

    mysqli_execute(
        mysql,
        "UPDATE user SET title = \"%s\", info = \"%s\" WHERE id = %d",
        title.c_str(),
        info.c_str(),
        atoi(argv[0].c_str())
    );

    quickSendMsg(200);
};