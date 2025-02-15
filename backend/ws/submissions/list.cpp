auto WSSubmissionsList = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    Json::Value submitIds = json_decode(ws_recv(conn));
    if (submitIds.size() == 0) {
        ws_exitRequest(conn);
        pthread_exit(NULL);
    }

    auto mysql = quick_mysqli_connect();
    auto submission = mysqli_query(
        mysql,
        "SELECT contest FROM submission WHERE id = %d",
        submitIds[0].asInt()
    );
    if (submission.size() == 0) {
        ws_exitRequest(conn);
        pthread_exit(NULL);
    }
    auto contest = mysqli_query(
        mysql,
        "SELECT type, starttime, duration FROM contest WHERE id = %d",
        atoi(submission[0]["contest"].c_str())
    );
    bool ended = contest.size() == 0 || time(NULL) > atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str());
    bool hidden = contest.size() && atoi(contest[0]["type"].c_str()) == 0;
    mysqli_close(mysql);

    Connection client = Client(appConfig["judge.sock"].asString()).connect();
    Json::Value fetchData;
    fetchData["type"] = SimpleSubmissionsLongConnection;
    fetchData["items"] = submitIds;
    client.send(json_encode(fetchData));

    map<int, bool> alive;
    for (int i = 0; i < submitIds.size(); i++) alive[submitIds[i].asInt()] = true;
    bool ws_alive = true;

    int count = 0, ws_count = 0;
    while(true) {
        string data = client.recv();
        if (data == errorKey) break;
        Json::Value val = json_decode(data);

        // 对特定比赛进行数据修改
        if (alive[val["sid"].asInt()]) {
            switch(val["type"].asInt()) {
                case DetailedSubmission: {
                    if (hidden && !ended) {
                        if (val["status"].asInt() != Waiting && val["status"].asInt() != Compiling && val["status"].asInt() != CE) {
                            val["memory"] = 0;
                            val["score"] = 100;
                            val["status"] = Submitted;
                            val["subtasks"].resize(0);
                            val["time"] = 0;
                        }
                    } if (!ended || !hasPermission(userInfo, SubmissionData)) {
                        for (int i = 0; i < val["subtasks"].size(); i++) {
                            for (int j = 0; j < val["subtasks"][i]["datas"].size(); j++) {
                                val["subtasks"][i]["datas"][j]["output"] = "";
                                val["subtasks"][i]["datas"][j]["outputIgnored"] = 0;
                            }
                        }
                    }
                }break;
                case UpdateSubmission: {
                    if (hidden && !ended) {
                        if (val["status"].asInt() != Waiting && val["status"].asInt() != Compiling && val["status"].asInt() != CE) {
                            val["memory"] = 0;
                            val["score"] = 100;
                            val["status"] = Submitted;
                            val["time"] = 0;
                        }
                    }
                }break;
                case UpdateSubtask: { // 理论上不应该存在
                    if (hidden && !ended) {
                        val = Json::objectValue;
                        writeLog(LOG_LEVEL_WARNING, "Unexpected UpdateSubtask");
                    }
                }break;
                case UpdateData: { // 理论上不应该存在
                    if (hidden && !ended) {
                        val = Json::objectValue;
                        writeLog(LOG_LEVEL_WARNING, "Unexpected UpdateData");
                    }
                    else if (!ended || !hasPermission(userInfo, SubmissionData)) {
                        val["output"] = "";
                        val["outputIgnored"] = 0;
                    }
                }break;
            }
        }

        if (ws_alive && alive[val["sid"].asInt()]) ws_send(conn, json_encode(val));
        if (val["type"].asInt() == UpdateSubmission) count++;
        if (
            alive[val["sid"].asInt()] && 
            val["type"].asInt() == DetailedSubmission &&
            hidden && !ended &&
            val["status"].asInt() != Waiting && val["status"].asInt() != Compiling && val["status"].asInt() != CE
        ) alive[val["sid"].asInt()] = false, ws_count++;
        if (ws_count == submitIds.size()) {
            ws_alive = false;
            ws_exitRequest(conn);
        }
        if (count == submitIds.size()) break;
    }

    if (ws_alive) ws_exitRequest(conn);
    pthread_exit(NULL);
};