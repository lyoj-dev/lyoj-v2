auto SubmissionsDetails = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();

    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM submission WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);
    auto problems = mysqli_query(
        mysql,
        "SELECT id, title FROM problem WHERE id = %d",
        atoi(res[0]["pid"].c_str())
    );
    if (problems.size() == 0) quickSendMsg(404);

    Json::Value fetchData;
    fetchData["type"] = DetailedSubmission;
    fetchData["item"] = atoi(argv[0].c_str());
    Connection client = Client(appConfig["judge.sock"].asString()).connect();
    client.send(json_encode(fetchData));
    Json::Value status = json_decode(client.recv());

    status["id"] = atoi(res[0]["id"].c_str());
    status["pid"] = atoi(res[0]["pid"].c_str());
    status["uid"] = atoi(res[0]["uid"].c_str());
    status["code"] = res[0]["code"];
    status["date"] = atoi(res[0]["time"].c_str());
    status["problem"] = problems[0]["title"];
    status["judged"] = atoi(res[0]["judged"].c_str());
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["item"] = status;

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};