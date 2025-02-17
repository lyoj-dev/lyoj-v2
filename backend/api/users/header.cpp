auto UsersHeader = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    /** 打开文件 */
    ifstream fin("data/users/" + to_string(atoi(argv[0].c_str())) + "/header.jpg", ios::binary);
    if (!fin.is_open()) quickSendMsgWithoutMySQL(404);

    /** 构造基础响应头 */
    argvar response = __default_response;
    response = merge(response, mime(".jpg"));

    /** 获取文件大小信息 */
    fin.seekg(0, ios::end);
    const __int64_t len = 1024 * 1024;
    __int64_t filesize = fin.tellg();
    __int64_t st = 0, en = filesize - 1;

    /** 获取断点续传范围信息 */
    if (request.argv.find("range") != request.argv.end()) {
        string range = explode("=", request.argv["range"].c_str())[1];
        param val = explode("-", range.c_str());
        st = atoi(val[0].c_str()), en = atoi(val[1].c_str());
        if (en == 0) en = filesize - 1;
    }

    /** 构造响应头并发送 */
    __int64_t output_len = en - st + 1;
    response["Content-Length"] = to_string(output_len);
    response["Content-Range"] = "bytes " + to_string(st) + "-" + to_string(en) + "/" + to_string(filesize);
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    putRequest(conn, request.argv.find("Range") != request.argv.end() ? 206 : 200, response);

    /** 从st起每次读取len长度的数据并发送 */
    char content[len] = "";
    memset(content, '\0', len);
    fin.seekg(st, ios::beg);
    while (output_len) {
        fin.read(content, min(len, output_len));
        int s = send(conn, content, min(len, output_len));
        if (s == -1) exitRequest(conn);
        output_len -= min(len, output_len);
    }
    exitRequest(conn);
};