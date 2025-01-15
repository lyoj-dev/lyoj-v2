auto ProblemsSubmit = [](client_conn conn, http_request request, param argv) {
    if (request.method != "POST") quickSendMsg(405);

    auto $_POST = json_decode(request.postdata);
    MYSQL mysql = quick_mysqli_connect();

    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM problem WHERE id = " + argv[0])[0]["count"].c_str());
    if (!exist) quickSendMsg(404);

    int id = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM submission")[0]["count"].c_str()) + 1;
    int uid = 0;
    int pid = atoi(argv[0].c_str());
    int lang = $_POST["lang"].asInt();
    string code = $_POST["code"].asString();
    string result = "{}";
    time_t time = std::time(NULL);
    int status = Waiting;
    int score = 0;
    bool judged = false;
    int contest = 0;

    mysqli_execute(
        mysql,
        "INSERT INTO submission VALUES (%d, %d, %d, %d, \"%s\", \"%s\", %d, %d, %d, %d, %d)",
        id, uid, pid, lang, quote_encode(code).c_str(), result.c_str(), time, status, score, judged, contest
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["id"] = id;

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};