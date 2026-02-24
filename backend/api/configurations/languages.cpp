#include "../../httpd.h"
#include "../../utils.cpp"

auto ConfigurationsLanguages = [](client_conn conn, http_request request, param argv) {
    Json::Value judge = json_decode(readFile("judge.json"));
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["items"] = judge["languages"];

    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};