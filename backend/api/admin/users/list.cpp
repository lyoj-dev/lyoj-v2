#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminUsersList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    if (!hasPermission(userInfo, AdminPage)) quickSendMsgWithoutMySQL(403);
    auto $_GET = getParam(request);

    std::string where = "id != 0";
    if ($_GET.find("title") != $_GET.end()) where += " AND title LIKE '%" + urldecode($_GET["title"]) + "%'";
    if ($_GET.find("groups") != $_GET.end()) where += " AND " + hasIntersection(std::string("groups"), json_decode($_GET["groups"]));
    if ($_GET.find("minRating") != $_GET.end())
        where += " AND rating >= " + std::to_string(atoi($_GET["minRating"].c_str()));
    if ($_GET.find("maxRating") != $_GET.end())
        where += " AND rating <= " + std::to_string(atoi($_GET["maxRating"].c_str()));

    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) AS count FROM user "
        "WHERE %s",
        where.c_str()
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql, 
        "SELECT id, title, createTime, rating, groups FROM user "
        "WHERE %s "
        "ORDER BY id ASC LIMIT 10 OFFSET %d", 
        where.c_str(),
        (page - 1) * 10
    ) : std::vector<argvar>();
    std::set<int> groupsSet;
    for (int i = 0; i < res.size(); i++) {
        Json::Value groups = json_decode(res[i]["groups"]);
        for (int j = 0; j < groups.size(); j++) if (groups[j].asInt()) groupsSet.insert(groups[j].asInt());
    }
    std::string groupsList = "";
    for (auto v : groupsSet) groupsList += std::to_string(v) + ",";
    if (groupsSet.size()) groupsList = groupsList.substr(0, groupsList.size() - 1);
    auto groups = groupsSet.size() ? mysqli_query(
        mysql,
        "SELECT * FROM userGroup WHERE id in (%s)",
        groupsList.c_str()
    ) : std::vector<std::map<std::string, std::string> >();

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
        single["createTime"] = atol(res[i]["createTime"].c_str());
        single["rating"] = atoi(res[i]["rating"].c_str());
        single["groups"] = Json::arrayValue;
        Json::Value required = json_decode(res[i]["groups"]);
        for (int j = 0; j < required.size(); j++) {
            for (int k = 0; k < groups.size(); k++) {
                if (groups[k]["id"] == required[j].asString()) {
                    Json::Value group;
                    group["id"] = atoi(groups[k]["id"].c_str());
                    group["title"] = groups[k]["title"];
                    group["description"] = groups[k]["description"];
                    group["permission"] = atoi(groups[k]["permission"].c_str());
                    single["groups"].append(group);
                    break;
                }
            }
        }
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