#include "../../httpd.h"
#include "../../utils.cpp"

auto ProblemsDataSingle = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM problem WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);

    // 权限检查
    if (argv.size() == 1 && !hasIntersection(json_decode(res[0]["groups"]), userInfo["groups"])) quickSendMsg(403);
    mysqli_close(mysql);

    // 存在性检查
    Json::Value config = json_decode(readFile("../problem/" + std::to_string(atoi(argv[0].c_str())) + "/config.json"));
    bool exist = false;
    for (int i = 0; i < config["datas"].size(); i++) exist |= 
        config["datas"][i]["input"].asString() == argv[1] ||
        config["datas"][i]["output"].asString() == argv[1];
    if (!exist) quickSendMsg(404);

    /** 打开文件 */
    std::ifstream fin("../problem/" + std::to_string(atoi(argv[0].c_str())) + "/" + argv[1], std::ios::binary);
    if (!fin.is_open()) quickSendMsg(404);

    /** 构造基础响应头 */
    argvar response = __default_response;
    response = merge(response, mime(".bin"));

    /** 获取文件大小信息 */
    fin.seekg(0, std::ios::end);
    const __int64_t len = 1024 * 1024;
    __int64_t filesize = fin.tellg();
    __int64_t st = 0, en = filesize - 1;

    /** 获取断点续传范围信息 */
    if (request.argv.find("Range") != request.argv.end()) {
        std::string range = explode("=", request.argv["Range"].c_str())[1];
        param val = explode("-", range.c_str());
        st = atoi(val[0].c_str()), en = atoi(val[1].c_str());
        if (en == 0) en = filesize - 1;
    }

    /** 构造响应头并发送 */
    __int64_t output_len = en - st + 1;
    response["Content-Length"] = std::to_string(output_len);
    response["Content-Range"] = "bytes " + std::to_string(st) + "-" + std::to_string(en) + "/" + std::to_string(filesize);
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    putRequest(conn, request.argv.find("Range") != request.argv.end() ? 206 : 200, response);

    /** 从st起每次读取len长度的数据并发送 */
    char content[len] = "";
    memset(content, '\0', len);
    fin.seekg(st, std::ios::beg);
    while (output_len) {
        fin.read(content, std::min(len, output_len));
        int s = send(conn, content, std::min(len, output_len));
        if (s == -1) exitRequest(conn);
        output_len -= std::min(len, output_len);
    }
    exitRequest(conn);
};