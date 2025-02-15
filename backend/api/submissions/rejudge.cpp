auto SubmissionsRejudge = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);

    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(mysql, "SELECT uid FROM submission WHERE id = " + argv[0]);
    if (res.size() == 0) quickSendMsg(404);
    if (atoi(res[0]["uid"].c_str()) == userId && !hasPermission(userInfo, SubmissionRejudge)) quickSendMsg(403);
    if (atoi(res[0]["uid"].c_str()) != userId && !hasPermission(userInfo, SubmissionRejudgeOthers)) quickSendMsg(403);

    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM submission WHERE id = " + argv[0])[0]["count"].c_str());
    if (!exist) quickSendMsg(404);

    mysqli_execute(
        mysql,
        "UPDATE submission SET judged = 0, status = %d, score = 0, result = \"{}\", time = %ld WHERE id = %d",
        Waiting, 
        time(NULL),
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