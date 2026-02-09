#include "../../httpd.h"
#include "../../utils.cpp"

auto UsersDetails = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    auto res = mysqli_query(
        mysql, 
        "SELECT * FROM user WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);
    int submissionsTotal = atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) as count FROM submission WHERE uid = %d AND contest = 0",
        atoi(argv[0].c_str())
    )[0]["count"].c_str());
    int submissionsAccepted = atoi(mysqli_query(
        mysql, 
        "SELECT COUNT(*) as count FROM submission WHERE uid = %d AND contest = 0 AND judged = true AND status = 0",
        atoi(argv[0].c_str())
    )[0]["count"].c_str());
    auto total = mysqli_query(
        mysql,
        "SELECT MAX(pid) AS pid FROM submission WHERE uid = %d AND contest = 0 AND judged = true GROUP BY pid",
        atoi(argv[0].c_str())
    );
    auto accepted = mysqli_query(
        mysql,
        "SELECT MAX(pid) AS pid FROM submission WHERE uid = %d AND contest = 0 AND judged = true AND status = 0 GROUP BY pid",
        atoi(argv[0].c_str())
    );
    std::string totalProblem = "", acceptedProblem = "";
    for (int i = 0; i < total.size(); i++) totalProblem += total[i]["pid"] + ",";
    for (int i = 0; i < accepted.size(); i++) acceptedProblem += accepted[i]["pid"] + ",";
    if (totalProblem.size()) totalProblem.pop_back();
    if (acceptedProblem.size()) acceptedProblem.pop_back();
    auto totalProblems = totalProblem == "" ? std::vector<std::map<std::string, std::string> >() : mysqli_query(
        mysql,
        "SELECT id, alias, title FROM problem WHERE id in (%s)",
        totalProblem.c_str()
    );
    auto acceptedProblems = acceptedProblem == "" ? std::vector<std::map<std::string, std::string> >() : mysqli_query(
        mysql,
        "SELECT id, alias, title FROM problem WHERE id in (%s)",
        acceptedProblem.c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["item"]["id"] = atoi(res[0]["id"].c_str());
    object["item"]["title"] = res[0]["title"];
    object["item"]["rating"] = atoi(res[0]["rating"].c_str());
    object["item"]["info"] = res[0]["info"];
    object["item"]["submissions"]["total"] = submissionsTotal;
    object["item"]["submissions"]["accepted"] = submissionsAccepted;
    object["item"]["problems"]["accepted"].resize(0);
    object["item"]["problems"]["tried"].resize(0);
    for (int i = 0; i < totalProblems.size(); i++) {
        Json::Value item;
        for (int j = 0; j < acceptedProblems.size(); j++)
            if (totalProblems[i]["id"] == acceptedProblems[j]["id"]) goto end;
        item["id"] = atoi(totalProblems[i]["id"].c_str());
        item["alias"] = totalProblems[i]["alias"];
        item["title"] = totalProblems[i]["title"];
        object["item"]["problems"]["tried"].append(item);
        end: continue;
    }
    for (int i = 0; i < acceptedProblems.size(); i++) {
        Json::Value item;
        item["id"] = atoi(acceptedProblems[i]["id"].c_str());
        item["alias"] = acceptedProblems[i]["alias"];
        item["title"] = acceptedProblems[i]["title"];
        object["item"]["problems"]["accepted"].append(item);
    }
    object["allowEdit"] = userId == atoi(argv[0].c_str()) ? true : hasPermission(userInfo, UserEditOthers);

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};