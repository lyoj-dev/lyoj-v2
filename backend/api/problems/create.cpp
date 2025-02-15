auto ProblemsCreate = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);

    auto $_POST = json_decode(request.postdata);
    int id = atoi(argv[0].c_str());
    string alias = $_POST["item"]["alias"].asString();
    string title = $_POST["item"]["title"].asString();
    string bg = $_POST["item"]["bg"].asString();
    string descrip = $_POST["item"]["descrip"].asString();
    string input = $_POST["item"]["input"].asString();
    string output = $_POST["item"]["output"].asString();
    string cases = json_encode($_POST["item"]["cases"]);
    string hint = $_POST["item"]["hint"].asString();
    bool hidden = false;
    bool banned = false;
    int difficulty = 0;
    string tags = json_encode($_POST["item"]["tags"]);
    int uid = userId;
    string groups = json_encode($_POST["item"]["groups"]);
    string langs = json_encode($_POST["item"]["langs"]);

    MYSQL mysql = quick_mysqli_connect();
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;

    if (id == 0) {
        id = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM problem")[0]["count"].c_str()) + 1;
        if (!hasPermission(userInfo, ProblemEdit)) quickSendMsg(403);
        mysqli_execute(
            mysql,
            "INSERT INTO problem VALUES (%d, \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", %d, %d, %d, \"%s\", %d, \"%s\", \"%s\")",
            id,
            quote_encode(alias).c_str(),
            quote_encode(title).c_str(),
            quote_encode(bg).c_str(),
            quote_encode(descrip).c_str(),
            quote_encode(input).c_str(),
            quote_encode(output).c_str(),
            quote_encode(cases).c_str(),
            quote_encode(hint).c_str(),
            hidden,
            banned,
            difficulty,
            quote_encode(tags).c_str(),
            uid,
            quote_encode(groups).c_str(),
            quote_encode(langs).c_str()
        );
        object["id"] = id;

        mkdir(("../problem/" + to_string(id)).c_str(), 0777);
        ofstream fout("../problem/" + to_string(id) + "/config.json");
        fout << json_encode($_POST["config"]);
        fout.close();
    } else {
        auto res = mysqli_query(mysql, "SELECT uid FROM problem WHERE id = " + argv[0]);
        if (res.size() == 0) quickSendMsg(404);
        if (atoi(res[0]["uid"].c_str()) == userId && !hasPermission(userInfo, ProblemEdit)) quickSendMsg(403);
        if (atoi(res[0]["uid"].c_str()) != userId && !hasPermission(userInfo, ProblemEditOthers)) quickSendMsg(403);
        mysqli_execute(
            mysql,
            "UPDATE problem SET alias = \"%s\", title = \"%s\", bg = \"%s\", descrip = \"%s\", input = \"%s\", output = \"%s\", cases = \"%s\", hint = \"%s\", tags = \"%s\", groups = \"%s\", langs = \"%s\" WHERE id = %d",
            quote_encode(alias).c_str(),
            quote_encode(title).c_str(),
            quote_encode(bg).c_str(),
            quote_encode(descrip).c_str(),
            quote_encode(input).c_str(),
            quote_encode(output).c_str(),
            quote_encode(cases).c_str(),
            quote_encode(hint).c_str(),
            quote_encode(tags).c_str(),
            quote_encode(groups).c_str(),
            quote_encode(langs).c_str(),
            id
        );
        object["id"] = id;
        ofstream fout("../problem/" + to_string(id) + "/config.json");
        fout << json_encode($_POST["config"]);
        fout.close();
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