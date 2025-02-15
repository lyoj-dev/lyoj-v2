auto WSSubmissionsDetails = [](client_conn conn, http_request request, param argv) {
    auto mysql = quick_mysqli_connect();
    auto userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    auto submission = mysqli_query(
        mysql,
        "SELECT contest FROM submission WHERE id = %d",
        atoi(argv[0].c_str())
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
    fetchData["type"] = DetailedSubmissionLongConnection;
    fetchData["item"] = atoi(argv[0].c_str());
    client.send(json_encode(fetchData));
    bool ws_alive = true;

    while(true) {
        string data = client.recv();
        if (data == errorKey) break;
        Json::Value val = json_decode(data);

        // 对特定比赛进行数据修改
        if (ws_alive) {
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
                case UpdateSubtask: {
                    if (hidden && !ended) { // 理论上不应该存在
                        val = Json::objectValue;
                        writeLog(LOG_LEVEL_WARNING, "Unexpected UpdateSubtask");
                    }
                }break;
                case UpdateData: {
                    if (hidden && !ended) { // 理论上不应该存在
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

        if (ws_alive) ws_send(conn, json_encode(val));
        if (
            ws_alive && 
            val["type"].asInt() == DetailedSubmission &&
            hidden && !ended &&
            val["status"].asInt() != Waiting && val["status"].asInt() != Compiling && val["status"].asInt() != CE
        ) {
            ws_exitRequest(conn);
            ws_alive = false;
        }
        if (val["type"].asInt() == UpdateSubmission) break;
    }

    if (ws_alive) ws_exitRequest(conn);
    pthread_exit(NULL);
};