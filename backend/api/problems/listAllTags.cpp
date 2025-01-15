auto ProblemsListAllTags = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    
    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM tags GROUP BY title ORDER BY title ASC"
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        if (res[i]["id"] == "0") continue;
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["title"] = res[i]["title"];
        single["type"] = atoi(res[i]["type"].c_str());
        object["items"].append(single);
    }

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};