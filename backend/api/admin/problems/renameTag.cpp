#include "../../../httpd.h"
#include "../../../utils.cpp"
#include <cstdlib>

auto AdminProblemsRenameTag = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_POST = json_decode(request.postdata);

    MYSQL mysql = quick_mysqli_connect();
    int id = $_POST["id"].asInt();
    std::string title = $_POST["title"].asString();
    int type = $_POST["type"].asInt();
    
    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM tags WHERE id = %d", id)[0]["count"].c_str());
    if (!exist) quickSendMsg(404);

    mysqli_execute(mysql, "UPDATE tags SET title = \"%s\", type = %d WHERE id = %d", title.c_str(), type, id);

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};