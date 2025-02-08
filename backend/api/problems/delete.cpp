auto ProblemsDelete = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsg(405);
    if (userId == 0) quickSendMsg(401);

    MYSQL mysql = quick_mysqli_connect();

    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM problem WHERE id = " + argv[0])[0]["count"].c_str());
    if (!exist) quickSendMsg(404);

    // 权限检查
    auto res = mysqli_query(mysql, "SELECT uid, groups FROM problem WHERE id = " + argv[0]);
    if (!hasIntersection(json_decode(res[0]["groups"]), userInfo["groups"])) quickSendMsg(403);
    if (userId != atoi(res[0]["uid"].c_str())) quickSendMsg(403);

    mysqli_execute(
        mysql, 
        "UPDATE problem SET hidden = true, banned = true, groups = '[]' WHERE id = %d", 
        atoi(argv[0].c_str())
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};