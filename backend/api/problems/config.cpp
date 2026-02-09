#include "../../httpd.h"
#include "../../utils.cpp"

auto ProblemsConfig = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    const int lim = 128;

    auto res = mysqli_query(
        mysql, 
        "SELECT groups FROM problem WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);

    // 权限检查
    if (!hasIntersection(json_decode(res[0]["groups"]), userInfo["groups"])) quickSendMsg(403);

    Json::Value config = json_decode(readFile("../problem/" + argv[0] + "/config.json"));
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["item"] = config;

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};