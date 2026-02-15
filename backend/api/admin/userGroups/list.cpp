#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminUserGroupsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_GET = getParam(request);

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM userGroup"
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT * FROM userGroup WHERE 1 ORDER BY id ASC LIMIT 10 OFFSET %d", 
        (page - 1) * 10
    ) : std::vector<argvar>();

    Json::Value ids;
    for (int i = 0; i < res.size(); i++) ids.append(atoi(res[i]["id"].c_str()));
    auto userRes = mysqli_query(
        mysql, 
        "SELECT id, groups, title FROM user WHERE %s", 
        hasIntersection(std::string("groups"), ids).c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["pageCount"] = pageCount;
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = atoi(res[i]["id"].c_str());
        single["title"] = res[i]["title"];
        single["description"] = res[i]["description"];
        single["permission"] = atol(res[i]["permission"].c_str());
        single["users"].resize(0);
        int cnt = 0;
        Json::Value goal; goal.append(atoi(res[i]["id"].c_str()));
        for (int j = 0; j < userRes.size(); j++) {
            if (hasIntersection(json_decode(userRes[j]["groups"]), goal)) {
                cnt++;
                Json::Value user;
                user["uid"] = atoi(userRes[j]["id"].c_str());
                user["user"] = userRes[j]["title"];
                single["users"].append(user);
            };
        }
        single["userCount"] = cnt;
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