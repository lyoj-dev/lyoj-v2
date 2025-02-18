string getContent(string source, string prefix, string suffix, int pt = 0) {
    int st = source.find(prefix, pt) + prefix.size();
    int ed = source.find(suffix, st);
    return source.substr(st, ed - st);
}

vector<pair<string, string> > htmlEntities = {
    {"&", "&amp;"},
    {" ", "&nbsp;"},
    {">", "&gt;"},
    {"<", "&lt;"},
    {"\"", "&quot;"},
    {"'", "&apos;"}
};
string htmldecode(string s) {
    for (int i = 0; i < 256; i++) htmlEntities.push_back({string(1, i), "&#" + to_string(i) + ";"});
    for (auto entity : htmlEntities) s = str_replace(entity.second, entity.first, s);
    return s;
}

auto UsersCasLogin = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (userId != 0) quickSendMsgWithoutMySQL(400);
    auto $_GET = getParam(request);
    if ($_GET.find("ticket") == $_GET.end()) quickSendMsgWithoutMySQL(400);

    string ticket = $_GET["ticket"];

    Client client(appConfig["cas"]["host"].asString(), appConfig["cas"]["port"].asInt());
    Connection conn2 = client.connect();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL* ssl;
    if (appConfig["cas"]["sslIgnoreCert"].asBool()) SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    if (appConfig["cas"]["ssl"].asBool()) {
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, conn2.conn);
        if (SSL_connect(ssl) == -1) quickSendMsgWithoutMySQL(500);
    }
    stringstream ss;
    ss << "GET " << appConfig["cas"]["prefix"].asString() << "p3/serviceValidate?ticket=" << ticket << "&service=" << urlencode(appConfig["cas"]["service"].asString()) << " HTTP/1.1\r\n";
    ss << "User-Agent: LittleYang OnlineJudge Backend Service\r\n";
    ss << "Host: " << appConfig["cas"]["host"].asString() << ":" << appConfig["cas"]["port"].asInt() << "\r\n";
    ss << "Connection: keep-alive\r\n\r\n";
    if (!appConfig["cas"]["ssl"].asBool()) send(conn2.conn, ss.str().c_str(), ss.str().size(), 0);
    else SSL_write(ssl, ss.str().c_str(), ss.str().size());
    char* ch = new char[1024 * 1024];
    int len = 0;
    if (!appConfig["cas"]["ssl"].asBool()) len = recv(conn2.conn, ch, 1024 * 1024, 0);
    else len = SSL_read(ssl, ch, 1024 * 1024);
    string r = string(ch, len);
    string data = r.substr(r.find("\r\n\r\n") + 4);
    delete[] ch;

    Json::Value object;
    object["code"] = 200;
    object["success"] = true;
    object["msg"] = "OK";
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    if (data.find("cas:authenticationFailure") != string::npos) {
        object["success"] = false;
        object["msg"] = "Authentication Failure";
        int pt = data.find("authenticationFailure") + string("authenticationFailure").size();
        object["failureCode"] = getContent(data, "code=\"", "\"", pt);
        object["failureMsg"] = htmldecode(getContent(data, ">", "<", pt));
    } else if (data.find("cas:authenticationSuccess") != string::npos) {
        // object["user"] = getContent(data, "<cas:user>", "</cas:user>");
        // object["credentialType"] = getContent(data, "<cas:credentialType>", "</cas:credentialType>");
        // object["clientIpAddress"] = getContent(data, "<cas:clientIpAddress>", "</cas:clientIpAddress>");
        // object["isFromNewLogin"] = getContent(data, "<cas:isFromNewLogin>", "</cas:isFromNewLogin>");   
        // object["authenticationDate"] = getContent(data, "<cas:authenticationDate>", "</cas:authenticationDate>");
        // object["authenticationMethod"] = getContent(data, "<cas:authenticationMethod>", "</cas:authenticationMethod>");
        // object["successfulAuthenticationHandlers"] = getContent(data, "<cas:successfulAuthenticationHandlers>", "</cas:successfulAuthenticationHandlers>");
        // object["serverIpAddress"] = getContent(data, "<cas:serverIpAddress>", "</cas:serverIpAddress>");
        // object["longTermAuthenticationRequestTokenUsed"] = getContent(data, "<cas:longTermAuthenticationRequestTokenUsed>", "</cas:longTermAuthenticationRequestTokenUsed>");
        // object["userAgent"] = getContent(data, "<cas:userAgent>", "</cas:userAgent>");
        string schoolId = "";

        MYSQL mysql = quick_mysqli_connect();
        auto res = mysqli_query(mysql, "SELECT * FROM user WHERE schoolId = \"%s\"", schoolId.c_str());
        int uid = 0;
        if (res.size() == 0) {

        } else uid = atoi(res[0]["id"].c_str());
        string session = generateSession();
        mysqli_execute(
            mysql,
            "INSERT INTO session VALUES (%d, \"%s\", %ld)",
            uid,
            session.c_str(),
            time(NULL)
        );
        object["session"] = session;
        mysqli_close(mysql);
    } else {
        object["success"] = false;
        object["msg"] = "Bad Request";
    }

    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};