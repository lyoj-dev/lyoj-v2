auto ContestsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    auto $_GET = getParam(request);
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    string where = "1";

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM contest "
        "WHERE %s",
        where.c_str()
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT * FROM contest "
        "WHERE %s "
        "ORDER BY id DESC LIMIT 10 OFFSET %d", 
        where.c_str(),
        (page - 1) * 10
    ) : vector<argvar>();
    string contestList = "";
    for (int i = 0; i < res.size(); i++) contestList += (i ? "," : "") + res[i]["id"];
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

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["pageCount"] = pageCount;
    object["allowCreate"] = hasPermission(userInfo, ContestEdit);
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["title"] = res[i]["title"];
        single["starttime"] = atoi(res[i]["starttime"].c_str());
        single["duration"] = atoi(res[i]["duration"].c_str());
        single["type"] = atoi(res[i]["type"].c_str());
        single["rated"] = atoi(res[i]["rated"].c_str());
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
        single["problems"] = json_decode(res[i]["problems"]).size();
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