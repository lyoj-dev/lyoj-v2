auto ProblemsDetails = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM problem WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);

    // 权限检查
    if (argv.size() == 1 && !hasIntersection(json_decode(res[0]["groups"]), userInfo["groups"])) quickSendMsg(403);

    auto submission = mysqli_query(
        mysql,
        "SELECT lang, code FROM submission WHERE uid = %d AND pid = %d AND contest = %d ORDER BY id DESC LIMIT 1",
        userId,
        atoi(argv[0].c_str()),
        argv.size() == 2 ? atoi(argv[1].c_str()) : 0
    );
    string code = submission.size() ? submission[0]["code"] : "";
    int lang = submission.size() ? atoi(submission[0]["lang"].c_str()) : 0;

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    if (argv.size() == 2) {
        object["identity"] = getContestIdentity(userId, atoi(argv[1].c_str()));
        object["signup"] = checkSignUp(userId, atoi(argv[1].c_str()));
    }
    object["item"]["id"] = atoi(res[0]["id"].c_str());
    object["item"]["alias"] = res[0]["alias"];
    object["item"]["title"] = res[0]["title"];
    object["item"]["difficulty"] = atoi(res[0]["difficulty"].c_str());
    object["item"]["tags"] = Json::arrayValue;
    auto tags = res[0]["tags"] == "[]" ? vector<argvar>() : mysqli_query(
        mysql,
        "SELECT * FROM tags WHERE id in (%s)",
        res[0]["tags"].substr(1, res[0]["tags"].size() - 2).c_str()
    );
    for (int i = 0; i < tags.size(); i++) {
        Json::Value single;
        single["id"] = atoi(tags[i]["id"].c_str());
        single["title"] = tags[i]["title"];
        single["type"] = atoi(tags[i]["type"].c_str());
        object["item"]["tags"].append(single);
    }
    object["item"]["bg"] = res[0]["bg"];
    object["item"]["descrip"] = res[0]["descrip"];
    object["item"]["input"] = res[0]["input"];
    object["item"]["output"] = res[0]["output"];
    object["item"]["cases"] = json_decode(res[0]["cases"]);
    object["item"]["hint"] = res[0]["hint"];
    object["item"]["langs"] = json_decode(res[0]["langs"]);
    object["item"]["groups"] = json_decode(res[0]["groups"]);
    object["item"]["lastCode"] = code;
    object["item"]["lastLang"] = lang;

    Json::Value config = json_decode(readFile("../problem/" + argv[0] + "/config.json"));
    object["config"]["input"] = config["input"];
    object["config"]["output"] = config["output"];
    object["config"]["minTime"] = [&](){
        int _min = 1e9;
        for (int i = 0; i < config["datas"].size(); i++) _min = min(_min, config["datas"][i]["time"].asInt());
        return _min;
    }();
    object["config"]["maxTime"] = [&](){
        int _max = 0;
        for (int i = 0; i < config["datas"].size(); i++) _max = max(_max, config["datas"][i]["time"].asInt());
        return _max;
    }();
    object["config"]["minMemory"] = [&](){
        int _min = 1e9;
        for (int i = 0; i < config["datas"].size(); i++) _min = min(_min, config["datas"][i]["memory"].asInt());
        return _min;
    }();
    object["config"]["maxMemory"] = [&](){
        int _max = 0;
        for (int i = 0; i < config["datas"].size(); i++) _max = max(_max, config["datas"][i]["memory"].asInt());
        return _max;
    }();

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};