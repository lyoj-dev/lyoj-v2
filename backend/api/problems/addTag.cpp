#include "../../httpd.h"
#include "../../utils.cpp"

auto ProblemsAddTag = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (!hasPermission(userInfo, AddTag)) quickSendMsgWithoutMySQL(403);
    Json::Value $_POST = json_decode(request.postdata);
    std::string title = $_POST["title"].asString();
    int type = $_POST["type"].asInt();

    MYSQL mysql = quick_mysqli_connect();
    if (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM tags WHERE title = \"%s\"",
        quote_encode(title).c_str()
    ).at(0)["count"].c_str())) quickSendMsg(400);

    int id = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM tags")[0]["count"].c_str()) + 1;
    mysqli_execute(
        mysql, 
        "INSERT INTO tags VALUES (%d, \"%s\", %d)", 
        id, 
        quote_encode(title).c_str(), 
        type
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["id"] = id;

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};