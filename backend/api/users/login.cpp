string generateSession(int len = 64) {
    string session = "";
    for (int i = 0; i < len; i++) {
        int t = rand() % 62;
        if (t < 10) session += '0' + t;
        else if (t < 36) session += 'A' + t - 10;
        else session += 'a' + t - 36;
    }
    return session;
}

auto UsersLogin = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);

    auto $_POST = json_decode(request.postdata);
    MYSQL mysql = quick_mysqli_connect();

    string user = $_POST["user"].asString();
    string passwd = $_POST["passwd"].asString();
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["success"] = true;

    auto res = mysqli_query(
        mysql,
        "SELECT * FROM user WHERE (title = \"%s\" OR schoolId = \"%s\") AND id != 0",
        quote_encode(user.c_str()).c_str(),
        quote_encode(user.c_str()).c_str()
    );
    if (res.size() == 0) {
        object["success"] = false;
        object["failMsg"] = "pages.login.userNotFound";
    }
    if (object["success"].asBool() && res.size() == 2) quickSendMsg(400);
    if (object["success"].asBool() && res[0]["password"] == "") {
        object["success"] = false;
        object["failMsg"] = "pages.login.disabledPassword";
    }
    if (object["success"].asBool() && res[0]["password"] != passwd) {
        object["success"] = false;
        object["failMsg"] = "pages.login.wrongPassword";
    }
    
    if (object["success"].asBool()) {
        string session = generateSession();
        mysqli_execute(
            mysql,
            "INSERT INTO session VALUES (%d, \"%s\", %lld)",
            atoi(res[0]["id"].c_str()),
            session.c_str(),
            time(NULL)
        );
        object["session"] = session;
    }

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};