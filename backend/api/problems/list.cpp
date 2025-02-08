auto ProblemsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    auto $_GET = getParam(request);
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    string where = hasIntersection("groups", userInfo["groups"], false);
    if ($_GET.find("title") != $_GET.end()) where += " AND title LIKE '%" + urldecode($_GET["title"]) + "%'";
    if ($_GET.find("tags") != $_GET.end()) where += " AND " + hasIntersection(string("tags"), json_decode($_GET["tags"]));
    if ($_GET.find("minDiff") != $_GET.end())
        where += " AND difficulty >= " + to_string(atoi($_GET["minDiff"].c_str()));
    if ($_GET.find("maxDiff") != $_GET.end())
        where += " AND difficulty <= " + to_string(atoi($_GET["maxDiff"].c_str()));

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM problem "
        "WHERE %s",
        where.c_str()
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT id, alias, title, difficulty, tags, uid FROM problem "
        "WHERE %s "
        "ORDER BY id DESC LIMIT 10 OFFSET %d", 
        where.c_str(),
        (page - 1) * 10
    ) : vector<argvar>();
    string problemList = "";
    for (int i = 0; i < res.size(); i++) problemList += (i ? "," : "") + res[i]["id"];
    set<int> tagsSet;
    for (int i = 0; i < res.size(); i++) {
        Json::Value tags = json_decode(res[i]["tags"]);
        for (int j = 0; j < tags.size(); j++) if (tags[j].asInt()) tagsSet.insert(tags[j].asInt());
    }
    string tagsList = "";
    for (auto v : tagsSet) tagsList += to_string(v) + ",";
    if (tagsSet.size()) tagsList = tagsList.substr(0, tagsList.size() - 1);
    auto tags = tagsSet.size() ? mysqli_query(
        mysql,
        "SELECT * FROM tags WHERE id in (%s)",
        tagsList.c_str()
    ) : vector<map<string, string> >();
    auto accepted = pageCount ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE judged = true AND status = 0 AND pid in (%s) GROUP BY pid",
        problemList.c_str()
    ) : vector<map<string, string> >();
    auto myAccepted = pageCount ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE judged = true AND status = 0 AND uid = %d AND pid in (%s) GROUP BY pid",
        userId,
        problemList.c_str()
    ) : vector<map<string, string> >();
    auto total = pageCount ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE pid in (%s) GROUP BY pid",
        problemList.c_str()
    ) : vector<map<string, string> >();
    auto myTotal = pageCount ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE uid = %d AND pid in (%s) GROUP BY pid",
        userId,
        problemList.c_str()
    ) : vector<map<string, string> >();

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
        single["alias"] = res[i]["alias"];
        single["title"] = res[i]["title"];
        single["difficulty"] = atoi(res[i]["difficulty"].c_str());
        single["tags"] = Json::arrayValue;
        Json::Value required = json_decode(res[i]["tags"]);
        for (int j = 0; j < required.size(); j++) {
            for (int k = 0; k < tags.size(); k++) {
                if (tags[k]["id"] == required[j].asString()) {
                    Json::Value tag;
                    tag["id"] = atoi(tags[k]["id"].c_str());
                    tag["title"] = tags[k]["title"];
                    tag["type"] = atoi(tags[k]["type"].c_str());
                    single["tags"].append(tag);
                    break;
                }
            }
        }
        single["accepted"] = 0;
        single["total"] = 0;
        for (int j = 0; j < accepted.size(); j++) 
            if (accepted[j]["pid"] == res[i]["id"]) single["accepted"] = atoi(accepted[j]["count"].c_str());
        for (int j = 0; j < total.size(); j++)
            if (total[j]["pid"] == res[i]["id"]) single["total"] = atoi(total[j]["count"].c_str());
        int myAcceptedCount = 0, myTotalCount = 0;
        for (int j = 0; j < myAccepted.size(); j++)
            if (myAccepted[j]["pid"] == res[i]["id"]) myAcceptedCount = atoi(myAccepted[j]["count"].c_str());
        for (int j = 0; j < myTotal.size(); j++)
            if (myTotal[j]["pid"] == res[i]["id"]) myTotalCount = atoi(myTotal[j]["count"].c_str());
        single["status"] = myTotalCount ? (myAcceptedCount ? 1 : 2) : 0;
        single["allowEdit"] = userId == atoi(res[i]["uid"].c_str());
        single["allowDelete"] = userId == atoi(res[i]["uid"].c_str());
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