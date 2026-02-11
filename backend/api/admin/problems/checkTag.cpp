#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminProblemsCheckTag = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_GET = getParam(request);

    MYSQL mysql = quick_mysqli_connect();
    int id = std::atoi($_GET["id"].c_str());
    
    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM tags WHERE id = %d", id)[0]["count"].c_str());
    if (!exist) quickSendMsg(404);
    Json::Value goal; goal.append($_GET["id"]);

    auto problems = mysqli_query(
        mysql, "SELECT id, tags FROM problem WHERE %s AND banned = 0", 
        hasIntersection(std::string("tags"), goal).c_str()
    );
    auto contests = mysqli_query(
        mysql, "SELECT id, tags FROM contest WHERE %s", 
        hasIntersection(std::string("tags"), goal).c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["count"]["problem"] = problems.size();
    object["count"]["contest"] = contests.size();
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