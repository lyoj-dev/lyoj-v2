#include "../../httpd.h"
#include "../../utils.cpp"

auto ContestsCreate = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);

    auto $_POST = json_decode(request.postdata);
    int id = atoi(argv[0].c_str());
    std::string title = $_POST["title"].asString();
    time_t starttime = $_POST["starttime"].asInt64();
    time_t duration = $_POST["duration"].asInt64();
    int type = $_POST["type"].asInt();
    bool rated = $_POST["rated"].asBool();
    std::string tags = json_encode($_POST["tags"]);
    std::string problems = json_encode($_POST["problems"]);
    std::string password = $_POST["password"].asString();
    std::string info = $_POST["info"].asString();
    int uid = userId;

    MYSQL mysql = quick_mysqli_connect();
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;

    if (id == 0) {
        id = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM contest")[0]["count"].c_str()) + 1;
        if (!hasPermission(userInfo, ContestEdit)) quickSendMsg(403);
        mysqli_execute(
            mysql,
            "INSERT INTO contest VALUES (%d, \"%s\", %ld, %ld, %d, %d, \"%s\", \"%s\", \"%s\", \"%s\", %d)",
            id,
            quote_encode(title).c_str(),
            starttime,
            duration,
            type,
            rated,
            quote_encode(tags).c_str(),
            quote_encode(problems).c_str(),
            quote_encode(password).c_str(),
            quote_encode(info).c_str(),
            uid
        );
        object["id"] = id;
    } else {
        auto res = mysqli_query(mysql, "SELECT uid FROM contest WHERE id = %d", id);
        if (res.size() == 0) quickSendMsg(404);
        if (atoi(res[0]["uid"].c_str()) == userId && !hasPermission(userInfo, ContestEdit)) quickSendMsg(403);
        if (atoi(res[0]["uid"].c_str()) != userId && !hasPermission(userInfo, ContestEditOthers)) quickSendMsg(403);
        mysqli_execute(
            mysql,
            "UPDATE contest SET title = \"%s\", starttime = %ld, duration = %ld, type = %d, rated = %d, tags = \"%s\", problems = \"%s\", password = \"%s\", info = \"%s\" WHERE id = %d",
            quote_encode(title).c_str(),
            starttime,
            duration,
            type,
            rated,
            quote_encode(tags).c_str(),
            quote_encode(problems).c_str(),
            quote_encode(password).c_str(),
            quote_encode(info).c_str(),
            id
        );
        object["id"] = id;
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