#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminUserGroupsCreate = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_POST = json_decode(request.postdata);

    int id = $_POST["id"].asInt();
    std::string title = $_POST["title"].asString();
    std::string description = $_POST["description"].asString();
    Json::Value users = $_POST["users"];
    int permission = $_POST["permission"].asInt();

    if (id == 0) {
        id = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM userGroup")[0]["count"].c_str()) + 1;
        mysqli_execute(
            mysql,
            "INSERT INTO userGroup VALUES (%d, \"%s\", \"%s\", %d)",
            id,
            quote_encode(title).c_str(),
            quote_encode(description).c_str(),
            permission
        );
    } else {
        mysqli_execute(
            mysql,
            "UPDATE userGroup SET title = \"%s\", description = \"%s\", permission = %d WHERE id = %d",
            quote_encode(title).c_str(),
            quote_encode(description).c_str(),
            permission,
            id
        );
    }

    std::string ids = json_encode(users);
    ids[0] = '('; ids[ids.size() - 1] = ')';
    if (users.size() == 0) ids = "1";
    else ids = "id in " + ids;
    Json::Value goal; goal.append(id);
    auto res = mysqli_query(
        mysql,
        "SELECT id, groups FROM user WHERE %s OR %s",
        ids.c_str(),
        hasIntersection(std::string("groups"), goal).c_str()
    );
    for (int i = 0; i < res.size(); i++) {
        Json::Value oldGroups = json_decode(res[i]["groups"]);
        Json::Value newGroups;
        bool has = false;
        Json::Value goal; goal.append(atoi(res[i]["id"].c_str()));
        for (int j = 0; j < oldGroups.size(); j++) {
            if (oldGroups[j].asInt() != id) newGroups.append(oldGroups[j].asInt());
            else {
                if (hasIntersection(users, goal)) has = true, newGroups.append(id);
                else continue;
            }
        }
        if (hasIntersection(users, goal) && !has) newGroups.append(id);
        mysqli_execute(
            mysql,
            "UPDATE user SET groups = \"%s\" WHERE id = %d",
            json_encode(newGroups).c_str(),
            atoi(res[i]["id"].c_str())
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