auto ContestsDetails = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    bool signup = checkSignUp(userId, atoi(argv[0].c_str()));

    auto res = mysqli_query(
        mysql,
        "SELECT * FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    set<int> tagsSet;
    if (res.size() == 0) quickSendMsg(404);
    Json::Value itemTags = json_decode(res[0]["tags"]);
    for (int j = 0; j < itemTags.size(); j++) if (itemTags[j].asInt()) tagsSet.insert(itemTags[j].asInt());
    string tagsList = "";
    for (auto v : tagsSet) tagsList += to_string(v) + ",";
    if (tagsSet.size()) tagsList = tagsList.substr(0, tagsList.size() - 1);
    auto tags = tagsSet.size() ? mysqli_query(
        mysql,
        "SELECT * FROM tags WHERE id in (%s)",
        tagsList.c_str()
    ) : vector<map<string, string> >();
    int signups = atoi(mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count FROM contest_signup WHERE id = %d",
        atoi(argv[0].c_str())
    )[0]["count"].c_str());

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["identity"] = identity;
    object["signup"] = signup;
    object["item"]["id"] = atoi(res[0]["id"].c_str());
    object["item"]["title"] = res[0]["title"];
    object["item"]["starttime"] = atoi(res[0]["starttime"].c_str());
    object["item"]["duration"] = atoi(res[0]["duration"].c_str());
    object["item"]["type"] = atoi(res[0]["type"].c_str());
    object["item"]["rated"] = atoi(res[0]["rated"].c_str());
    object["item"]["info"] = res[0]["info"];
    object["item"]["tags"] = Json::arrayValue;
    Json::Value required = json_decode(res[0]["tags"]);
    for (int j = 0; j < required.size(); j++) {
        for (int k = 0; k < tags.size(); k++) {
            if (tags[k]["id"] == required[j].asString()) {
                Json::Value tag;
                tag["id"] = atoi(tags[k]["id"].c_str());
                tag["title"] = tags[k]["title"];
                tag["type"] = atoi(tags[k]["type"].c_str());
                object["item"]["tags"].append(tag);
                break;
            }
        }
    }
    object["item"]["signups"] = signups;
    object["item"]["problemNumber"] = json_decode(res[0]["problems"]).size();
    object["item"]["allowEdit"] = identity == Administrator;
    object["item"]["allowDelete"] = identity == Administrator;
    object["item"]["hasPassword"] = res[0]["password"] != "";

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};