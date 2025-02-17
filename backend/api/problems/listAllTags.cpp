auto ProblemsListAllTags = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    
    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM tags GROUP BY title ORDER BY title ASC"
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        if (res[i]["id"] == "0") continue;
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["title"] = res[i]["title"];
        single["type"] = atoi(res[i]["type"].c_str());
        object["items"].append(single);
    }
    object["allowAddTag"] = hasPermission(userInfo, AddTag);

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};