#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminUsersCreate = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_POST = json_decode(request.postdata);

    MYSQL mysql = quick_mysqli_connect();
    int id = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM user")[0]["count"].c_str()) + 1;
    mysqli_execute(
        mysql,
        "INSERT INTO user VALUES (%d, \"%s\", \"%s\", \"%s\", %d, \"%s\", %d, \"%s\", %d, \"%s\", %d, 0, \"%s\", \"\")",
        id,
        quote_encode($_POST["title"].asString()).c_str(),
        quote_encode($_POST["name"].asString()).c_str(),
        quote_encode($_POST["idNumber"].asString()).c_str(),
        $_POST["idType"].asInt(),
        quote_encode($_POST["typeName"].asString()).c_str(),
        $_POST["unitId"].asInt(),
        quote_encode($_POST["unitName"].asString()).c_str(),
        $_POST["sex"].asInt(),
        quote_encode($_POST["password"].asString()).c_str(),
        int(time(NULL)),
        json_encode($_POST["groups"]).c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["id"] = id;
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