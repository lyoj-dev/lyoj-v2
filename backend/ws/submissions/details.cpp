auto WSSubmissionsDetails = [](client_conn conn, http_request request, param argv) {
    Connection client = Client(appConfig["judge.sock"].asString()).connect();
    Json::Value fetchData;
    fetchData["type"] = DetailedSubmissionLongConnection;
    fetchData["item"] = atoi(argv[0].c_str());
    client.send(json_encode(fetchData));

    while(true) {
        string data = client.recv();
        if (data == errorKey) break;
        Json::Value val = json_decode(data);
        ws_send(conn, data);
        if (val["type"].asInt() == UpdateSubmission) break;
    }

    ws_exitRequest(conn);
    pthread_exit(NULL);
};