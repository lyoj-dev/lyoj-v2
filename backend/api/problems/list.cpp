auto ProblemsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    auto $_GET = getParam(request);

    string where = "1";
    if ($_GET.find("title") != $_GET.end()) where += " AND A.title LIKE '%" + urldecode($_GET["title"]) + "%'";
    if ($_GET.find("tags") != $_GET.end()) {
        where += " AND B.id in (";
        Json::Value tags = json_decode($_GET["tags"]);
        for (int i = 0; i < tags.size(); i++) where += (i ? "," : "") + tags[i].asString();
        where += ")";
    }
    if ($_GET.find("difficulties") != $_GET.end()) {
        where += " AND A.difficulty in (";
        Json::Value difficulties = json_decode($_GET["difficulties"]);
        for (int i = 0; i < difficulties.size(); i++) where += (i ? "," : "") + difficulties[i].asString();
        where += ")";
    }

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(DISTINCT A.id) AS count FROM problem A "
        "JOIN tags B ON FIND_IN_SET(B.id, REPLACE(REPLACE(A.tags, ']', ''), '[', '')) "
        "WHERE %s",
        where.c_str()
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT A.id AS id, A.alias, A.title, A.difficulty, A.tags FROM problem A "
        "JOIN tags B ON FIND_IN_SET(B.id, REPLACE(REPLACE(A.tags, ']', ''), '[', '')) "
        "WHERE %s "
        "GROUP BY A.id "
        "ORDER BY A.id DESC LIMIT 10 OFFSET %d", 
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
    auto total = pageCount ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE pid in (%s) GROUP BY pid",
        problemList.c_str()
    ) : vector<map<string, string> >();

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
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
        single["status"] = 0;
        object["items"].append(single);
    }

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};