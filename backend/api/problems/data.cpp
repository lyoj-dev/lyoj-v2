auto ProblemsData = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    const int lim = 128;

    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM problem WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);

    Json::Value config = json_decode(readFile("../problem/" + argv[0] + "/config.json"));
    map<int, int> subtasks = { { 0, 0 } };
    for (int i = 0; i < config["subtasks"].size(); i++)
        subtasks[config["subtasks"][i]["id"].asInt()] = i + 1;
    Json::Value datas = Json::arrayValue;
    for (int i = 0; i < config["datas"].size(); i++) {
        Json::Value data;

        ifstream fin("../problem/" + argv[0] + "/" + config["datas"][i]["input"].asString());
        fin.seekg(0, ios::end);
        int len = fin.tellg();
        fin.seekg(0, ios::beg);
        char* ch = new char[min(lim, len)];
        fin.read(ch, min(lim, len));
        data["inputIgnored"] = max(0, len - lim);
        data["input"] = string(ch, min(lim, len)) + (len > lim ? "..." : "");
        fin.close();

        fin.open("../problem/" + argv[0] + "/" + config["datas"][i]["output"].asString());
        fin.seekg(0, ios::end);
        len = fin.tellg();
        fin.seekg(0, ios::beg);
        delete[] ch;
        ch = new char[min(lim, len)];
        fin.read(ch, min(lim, len));
        data["outputIgnored"] = max(0, len - lim);
        data["output"] = string(ch, min(lim, len)) + (len > lim ? "..." : "");
        fin.close();
        delete[] ch;

        data["inputName"] = config["datas"][i]["input"].asString();
        data["outputName"] = config["datas"][i]["output"].asString();
        datas[subtasks[config["datas"][i]["subtask"].asInt()]].append(data);
    }
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["items"] = datas;

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};