auto WSSubmissionsList = [](client_conn conn, http_request request, param argv) {
    Json::Value submitIds = json_decode(ws_recv(conn));

    Connection client = Client(appConfig["judge.sock"].asString()).connect();
    Json::Value fetchData;
    fetchData["type"] = SimpleSubmissionsLongConnection;
    fetchData["items"] = submitIds;
    client.send(json_encode(fetchData));

    int count = 0;
    while(true) {
        string data = client.recv();
        if (data == errorKey) break;
        Json::Value val = json_decode(data);
        ws_send(conn, data);
        if (val["type"].asInt() == UpdateSubmission) count++;
        if (count == submitIds.size()) break;
    }

    ws_exitRequest(conn);
    pthread_exit(NULL);
};