#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminProblemsRejudge = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_POST = json_decode(request.postdata);

    MYSQL mysql = quick_mysqli_connect();
    std::string ids = json_encode($_POST["ids"]);
    ids[0] = '(', ids[ids.size() - 1] = ')';

    auto details = mysqli_query(
        mysql,
        "SELECT * FROM problem WHERE id in %s", 
        ids.c_str()
    );
    if (details.size() != $_POST["ids"].size()) quickSendMsg(404);

    mysqli_execute(
        mysql,
        "UPDATE submission SET judged = 0, status = %d, score = 0, result = \"{}\", time = %ld WHERE pid in %s",
        Waiting, 
        time(NULL),
        ids.c_str()
    );

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