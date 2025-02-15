auto ContestsUsersListAll = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    bool signup = checkSignUp(userId, atoi(argv[0].c_str()));

    auto contest = mysqli_query(
        mysql,
        "SELECT id, uid, starttime, duration FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    auto res = mysqli_query(
        mysql,
        "SELECT uid FROM contest_signup WHERE id = %d",
        atoi(argv[0].c_str())
    );
    string where = "id in (";
    for (int i = 0; i < res.size(); i++) where += res[i]["uid"] + (i + 1 < res.size() ? "," : "");
    where += ")";
    if (res.size() == 0) where = "0";
    res = mysqli_query(
        mysql, 
        "SELECT id, title FROM user WHERE %s ORDER BY id ASC",
        where.c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["items"].resize(0);
    if (identity != Administrator) {
        if (time(NULL) < atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str())) {
            if (identity == Visitor) res = {};
            else {
                for (int i = 0; i < res.size(); i++) {
                    if (atoi(res[i]["id"].c_str()) != userId) continue;
                    res = { res[i] };
                    break;
                }
            }
        }
    }
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["title"] = res[i]["title"];
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