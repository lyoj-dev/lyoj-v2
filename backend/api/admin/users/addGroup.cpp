#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminUsersAddGroup = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_POST = json_decode(request.postdata);

    MYSQL mysql = quick_mysqli_connect();
    std::string ids = json_encode($_POST["ids"]);
    ids[0] = '(', ids[ids.size() - 1] = ')';
    
    auto users = mysqli_query(
        mysql,
        "SELECT id, groups FROM user WHERE id in %s", 
        ids.c_str()
    );
    if (users.size() != $_POST["ids"].size()) quickSendMsg(404);

    for (int i = 0; i < users.size(); i++) {
        Json::Value oldGroups = json_decode(users[i]["groups"]);
        std::set<int> groups = {};
        for (int j = 0; j < oldGroups.size(); j++) groups.insert(oldGroups[j].asInt());
        for (int j = 0; j < $_POST["groups"].size(); j++) groups.insert($_POST["groups"][j].asInt());

        Json::Value newGroups;
        for (auto v : groups) newGroups.append(v);
        mysqli_execute(
            mysql, 
            "UPDATE user SET groups = \"%s\" WHERE id = %d", 
            json_encode(newGroups).c_str(), 
            std::atoi(users[i]["id"].c_str())
        );
    }

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