auto SubmissionsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    auto $_GET = getParam(request);
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    string where = hasIntersection("p.groups", userInfo["groups"], false);
    if ($_GET.find("problems") != $_GET.end()) {
        where += " AND s.pid in (";
        Json::Value problems = json_decode($_GET["problems"]);
        for (int i = 0; i < problems.size(); i++) where += (i ? "," : "") + to_string(problems[i].asInt());
        where += ")";
    }
    if ($_GET.find("users") != $_GET.end()) {
        where += " AND s.uid in (";
        Json::Value users = json_decode($_GET["users"]);
        for (int i = 0; i < users.size(); i++) where += (i ? "," : "") + to_string(users[i].asInt());
        where += ")";
    }
    if ($_GET.find("languages") != $_GET.end()) {
        where += " AND s.lang in (";
        Json::Value languages = json_decode($_GET["languages"]);
        for (int i = 0; i < languages.size(); i++) where += (i ? "," : "") + to_string(languages[i].asInt());
        where += ")";
    }
    if ($_GET.find("status") != $_GET.end()) {
        where += " AND s.judged = true AND status in (";
        Json::Value status = json_decode($_GET["status"]);
        for (int i = 0; i < status.size(); i++) where += (i ? "," : "") + to_string(status[i].asInt());
        where += ")";
    }

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM submission AS s "
        "INNER JOIN problem AS p ON s.pid = p.id "
        "WHERE %s",
        where.c_str()
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT s.id, s.uid, s.pid, s.judged FROM submission AS s "
        "INNER JOIN problem AS p ON s.pid = p.id "
        "WHERE %s "
        "ORDER BY id DESC "
        "LIMIT 10 OFFSET %d", 
        where.c_str(),
        (page - 1) * 10
    ) : vector<map<string, string> >();
    string problemList = "";
    for (int i = 0; i < res.size(); i++) problemList += (i ? "," : "") + res[i]["pid"];
    auto problems = pageCount ? mysqli_query(
        mysql,
        "SELECT id, title FROM problem WHERE id in (%s)",
        problemList.c_str()
    ) : vector<map<string, string> >();
    string userList = "";
    for (int i = 0; i < res.size(); i++) userList += (i ? "," : "") + res[i]["uid"];
    auto users = pageCount ? mysqli_query(
        mysql,
        "SELECT id, title FROM user WHERE id in (%s)",
        userList.c_str()
    ) : vector<map<string, string> >();

    Json::Value fetchData;
    fetchData["type"] = SimpleSubmissions;
    fetchData["items"].resize(0);
    for (int i = 0; i < res.size(); i++) fetchData["items"].append(atoi(res[i]["id"].c_str()));
    Json::Value status = Json::arrayValue;
    if (pageCount) {
        Connection client = Client(appConfig["judge.sock"].asString()).connect();
        client.send(json_encode(fetchData));
        status = json_decode(client.recv());
    }
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["pageCount"] = pageCount;
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        for (int j = 0; j < users.size(); j++) if (users[j]["id"] == res[i]["uid"]) {
            single["user"] = users[j]["title"];
            single["uid"] = atoi(users[j]["id"].c_str());
            break;
        }
        for (int j = 0; j < problems.size(); j++) if (problems[j]["id"] == res[i]["pid"]) {
            single["problem"] = problems[j]["title"];
            single["pid"] = atoi(problems[j]["id"].c_str());
            break;
        }
        for (int j = 0; j < status.size(); j++) if (status[j]["id"].asString() == res[i]["id"]) {
            single["statusType"] = status[j]["statusType"].asInt();
            single["status"] = status[j]["status"].asString();
            single["judged"] = status[j]["judged"].asBool();
            single["score"] = status[j]["score"].asInt();
            break;
        }
        object["items"].append(single);
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