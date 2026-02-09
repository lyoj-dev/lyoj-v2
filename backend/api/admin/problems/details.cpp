#include "../../../httpd.h"
#include "../../../utils.cpp"
#include <cstdlib>
#include <string>

auto AdminProblemsDetails = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_GET = getParam(request);

    MYSQL mysql = quick_mysqli_connect();
    std::string ids = $_GET["ids"];
    ids[0] = '(', ids[ids.size() - 1] = ')';

    auto details = mysqli_query(
        mysql,
        "SELECT * FROM problem WHERE id in %s", 
        ids.c_str()
    );
    if (details.size() != json_decode($_GET["ids"]).size()) quickSendMsg(404);

    for (int i = 0; i < details.size(); i++) {
        int id = atoi(details[i]["id"].c_str());
        Json::Value config = json_decode(readFile("../problem/" + std::to_string(id) + "/config.json"));
        details[i]["inputFile"] = config["input"].asString();
        details[i]["outputFile"] = config["output"].asString();
    }

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;

    for (auto v : details[0]) {
        std::string key = v.first;
        std::string val = v.second;
        for (int i = 0; i < details.size(); i++) {
            if (val != details[i][key]) val = "";
        }
        if (val != "") {
            if (std::set<std::string>({
                "alias", "title", "bg", "descrip", "input", "output", "hint", "inputFile", "outputFile"
            }).count(key)) object["item"][key] = val;
            if (std::set<std::string>({
                "tags"
            }).count(key)) {
                auto tags = val == "[]" ? std::vector<argvar>() : mysqli_query(
                    mysql,
                    "SELECT * FROM tags WHERE id in (%s)",
                    val.substr(1, val.size() - 2).c_str()
                );
                for (int i = 0; i < tags.size(); i++) {
                    Json::Value single;
                    single["id"] = atoi(tags[i]["id"].c_str());
                    single["title"] = tags[i]["title"];
                    single["type"] = atoi(tags[i]["type"].c_str());
                    object["item"]["tags"].append(single);
                }
            }
            if (std::set<std::string>({
                "langs", "groups"
            }).count(key)) object["item"][key] = json_decode(val);
        }
    }

    for (int i = 0; i < details.size(); i++) {
        Json::Value single;
        single["id"] = atoi(details[i]["id"].c_str());
        single["title"] = details[i]["title"];
        single["alias"] = details[i]["alias"];
        object["origin"].append(single);
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