#include "../../../httpd.h"
#include "../../../utils.cpp"
#include <cstdlib>

auto AdminProblemsDeleteTag = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_POST = json_decode(request.postdata);

    MYSQL mysql = quick_mysqli_connect();
    int id = $_POST["id"].asInt();
    
    bool exist = atoi(mysqli_query(mysql, "SELECT COUNT(*) AS count FROM tags WHERE id = %d", id)[0]["count"].c_str());
    if (!exist) quickSendMsg(404);
    Json::Value goal; goal.append($_POST["id"]);

    auto problems = mysqli_query(
        mysql, "SELECT id, tags FROM problem WHERE %s", 
        hasIntersection(std::string("tags"), goal).c_str()
    );
    for (int i = 0; i < problems.size(); i++) {
        Json::Value oldTags = json_decode(problems[i]["tags"]);
        Json::Value newTags;
        for (int j = 0; j < oldTags.size(); j++) 
            if (oldTags[j].asInt() != id)
                newTags.append(oldTags[j].asInt());
        mysqli_execute(
            mysql, 
            "UPDATE problem SET tags = \"%s\" WHERE id = %d", 
            json_encode(newTags).c_str(), 
            std::atoi(problems[i]["id"].c_str())
        );
    }

    auto contests = mysqli_query(
        mysql, "SELECT id, tags FROM contest WHERE %s", 
        hasIntersection(std::string("tags"), goal).c_str()
    );
    for (int i = 0; i < contests.size(); i++) {
        Json::Value oldTags = json_decode(contests[i]["tags"]);
        Json::Value newTags;
        for (int j = 0; j < oldTags.size(); j++) 
            if (oldTags[j].asInt() != id)
                newTags.append(oldTags[j].asInt());
        mysqli_execute(
            mysql, 
            "UPDATE contest SET tags = \"%s\" WHERE id = %d", 
            json_encode(newTags).c_str(), 
            std::atoi(contests[i]["id"].c_str())
        );
    }

    mysqli_execute(mysql, "DELETE FROM tags WHERE id = %d", id);

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