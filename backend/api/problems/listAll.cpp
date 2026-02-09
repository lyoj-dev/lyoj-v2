#include "../../httpd.h"
#include "../../utils.cpp"

auto ProblemsListAll = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    
    std::string where = hasIntersection("groups", userInfo["groups"], false);
    auto res = mysqli_query(
        mysql, 
        "SELECT id, alias, title FROM problem WHERE %s ORDER BY id DESC",
        where.c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["alias"] = res[i]["alias"];
        single["title"] = res[i]["title"];
        object["items"].append(single);
    }

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};