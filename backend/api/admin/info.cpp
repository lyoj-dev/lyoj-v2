string trim(string s) {
    while (s.size() && s.front() == ' ') s = s.substr(1);
    while (s.size() && s.back() == ' ') s.pop_back();
    return s;
}

string system2(string cmd) {
    FILE *stream; 
    char buf[1024 * 1024]; 
    memset(buf, '\0', sizeof(buf));
#if __linux__
    stream = popen(cmd.c_str(), "r");
// #elif __windows__
//     stream = _popen(cmd.c_str(), "r");
#endif
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
    pclose(stream);
    return string(buf);
}

auto AdminInfo = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);

    Json::Value object;
    object["code"] = 200;
    object["msg"] = "OK";
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;

    // 计算 cpu 信息
    {
        string cpuInfo = system2("cat /proc/cpuinfo");
        auto lines = explode("\n", cpuInfo);
        int cores = 0;
        for (auto line : lines) {
            auto parts = explode(":", line);
            if (parts[0].find("model name") != string::npos) object["cpu"]["name"] = trim(parts[1]), cores++;
            if (parts[0].find("cpu MHz") != string::npos) object["cpu"]["speed"] = atof(trim(parts[1]).c_str());
        }
        object["cpu"]["cores"] = cores;

        string res = system2("top -bn 1 -i -c");
        lines = explode("\n", res);
        for (auto line : lines) {
            if (line.find("Cpu(s):") != string::npos) {
                auto parts = explode(":", line);
                auto parts2 = explode(",", parts[1]);
                for (int i = 0; i < parts2.size(); i++) {
                    auto parts3 = explode(" ", trim(parts2[i]));
                    if (parts3.size() >= 2) object["cpu"]["usage"][trim(parts3[1])] = atof(trim(parts3[0]).c_str());
                }
            }
        }
    }

    // 计算内存信息
    {
        string res = system2("free -m");
        auto lines = explode("\n", res);
        for (auto line : lines) {
            auto parts = explode(" ", line);
            int pt = 0;
            for (int i = 0; i < parts.size(); i++) if (parts[i] != "") parts[pt++] = parts[i];
            parts.resize(pt);
            if (parts[0] == "Mem:") {
                object["memory"]["mem"]["total"] = atoi(parts[1].c_str());
                object["memory"]["mem"]["used"] = atoi(parts[2].c_str());
                object["memory"]["mem"]["free"] = atoi(parts[3].c_str());
                object["memory"]["mem"]["shared"] = atoi(parts[4].c_str());
                object["memory"]["mem"]["cache"] = atoi(parts[5].c_str());
                object["memory"]["mem"]["available"] = atoi(parts[6].c_str());
            } else if (parts[0] == "Swap:") {
                object["memory"]["swap"]["total"] = atoi(parts[1].c_str());
                object["memory"]["swap"]["used"] = atoi(parts[2].c_str());
                object["memory"]["swap"]["free"] = atoi(parts[3].c_str());
            }
        }
    }

    // 计算磁盘信息
    {
        string res = system2("df");
        auto lines = explode("\n", res);
        object["disk"].resize(0);
        for (auto line : lines) {
            auto parts = explode(" ", line);
            int pt = 0;
            for (int i = 0; i < parts.size(); i++) if (parts[i] != "") parts[pt++] = parts[i];
            parts.resize(pt);
            if (parts[0] == "Filesystem") continue;
            if (parts.size() < 6) continue;
            Json::Value obj;
            parts[4].pop_back();
            obj["size"] = atol(parts[1].c_str());
            obj["used"] = atol(parts[2].c_str());
            obj["avail"] = atol(parts[3].c_str());
            obj["use"] = atol(parts[4].c_str()) * 1.0 / 100;
            obj["mounted"] = parts[5];
            object["disk"].append(obj);
        }
    }

    auto mysql = quick_mysqli_connect();
    object["problems"] = atoi(mysqli_query(mysql, "SELECT COUNT(*) as count FROM problem")[0]["count"].c_str());
    object["totalSubmissions"] = atoi(mysqli_query(mysql, "SELECT COUNT(*) as count FROM submission")[0]["count"].c_str());
    object["todaySubmissions"] = atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) as count FROM submission WHERE time >= %ld",
        [](){
            time_t now = time(NULL);
            struct tm *tm = localtime(&now);
            tm->tm_hour = 0;
            tm->tm_min = 0;
            tm->tm_sec = 0;
            return mktime(tm);
        }()
    )[0]["count"].c_str());
    object["contests"] = atoi(mysqli_query(mysql, "SELECT COUNT(*) as count FROM contest")[0]["count"].c_str());
    object["users"] = atoi(mysqli_query(mysql, "SELECT COUNT(*) as count FROM user")[0]["count"].c_str());
    object["tags"] = atoi(mysqli_query(mysql, "SELECT COUNT(*) as count FROM tags")[0]["count"].c_str());
    object["groups"] = atoi(mysqli_query(mysql, "SELECT COUNT(*) as count FROM userGroup")[0]["count"].c_str());

    mysqli_close(mysql);
    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};