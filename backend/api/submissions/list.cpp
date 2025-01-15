auto SubmissionsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    auto $_GET = getParam(request);

    string where = "1";
    if ($_GET.find("problems") != $_GET.end()) {
        where += " AND pid in (";
        Json::Value problems = json_decode($_GET["problems"]);
        for (int i = 0; i < problems.size(); i++) where += (i ? "," : "") + to_string(problems[i].asInt());
        where += ")";
    }
    if ($_GET.find("languages") != $_GET.end()) {
        where += " AND lang in (";
        Json::Value languages = json_decode($_GET["languages"]);
        for (int i = 0; i < languages.size(); i++) where += (i ? "," : "") + to_string(languages[i].asInt());
        where += ")";
    }
    if ($_GET.find("status") != $_GET.end()) {
        where += " AND judged = true AND status in (";
        Json::Value status = json_decode($_GET["status"]);
        for (int i = 0; i < status.size(); i++) where += (i ? "," : "") + to_string(status[i].asInt());
        where += ")";
    }

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM submission WHERE %s",
        where.c_str()
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT id, uid, pid, judged FROM submission WHERE %s ORDER BY id DESC LIMIT 10 OFFSET %d", 
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
    object["pageCount"] = pageCount;
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["uid"] = atoi(res[i]["uid"].c_str());
        single["user"] = res[i]["uid"];
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
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};