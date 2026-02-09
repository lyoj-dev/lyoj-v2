#include "../../../httpd.h"
#include "../../../utils.cpp"

auto ContestsProblemsList = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    auto $_GET = getParam(request);
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    bool signup = checkSignUp(userId, atoi(argv[0].c_str()));

    auto res = mysqli_query(
        mysql,
        "SELECT problems, starttime, duration, type, uid FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    std::set<int> tagsSet;
    if (res.size() == 0) quickSendMsg(404);
    int type = atoi(res[0]["type"].c_str());
    bool ended = time(NULL) > atol(res[0]["starttime"].c_str()) + atol(res[0]["duration"].c_str());
    auto problems = res[0]["problems"] == "[]" ? std::vector<argvar>() : mysqli_query(
        mysql,
        "SELECT id, alias, title, difficulty, tags, uid FROM problem WHERE id in (%s)",
        res[0]["problems"].substr(1, res[0]["problems"].size() - 2).c_str()
    );
    for (int i = 0; i < problems.size(); i++) {
        Json::Value tags = json_decode(problems[i]["tags"]);
        for (int j = 0; j < tags.size(); j++) if (tags[j].asInt()) tagsSet.insert(tags[j].asInt());
    }
    std::string tagsList = "";
    for (auto v : tagsSet) tagsList += std::to_string(v) + ",";
    if (tagsSet.size()) tagsList = tagsList.substr(0, tagsList.size() - 1);
    auto tags = tagsSet.size() ? mysqli_query(
        mysql,
        "SELECT * FROM tags WHERE id in (%s)",
        tagsList.c_str()
    ) : std::vector<std::map<std::string, std::string> >();
    int signups = atoi(mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count FROM contest_signup WHERE id = %d",
        atoi(argv[0].c_str())
    )[0]["count"].c_str());
    auto accepted = problems.size() ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE judged = true AND status in (%s) AND contest = %d AND pid in (%s) GROUP BY pid",
        type == OI && !ended ? "0,1,2,3,4,5,6,7,11,12,13" : "0",
        atoi(argv[0].c_str()),
        res[0]["problems"].substr(1, res[0]["problems"].size() - 2).c_str()
    ) : std::vector<std::map<std::string, std::string> >();
    auto myAccepted = problems.size() ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE judged = true AND status in (%s) AND uid = %d AND contest = %d AND pid in (%s) GROUP BY pid",
        type == OI && !ended ? "0,1,2,3,4,5,6,7,11,12,13" : "0",
        userId,
        atoi(argv[0].c_str()),
        res[0]["problems"].substr(1, res[0]["problems"].size() - 2).c_str()
    ) : std::vector<std::map<std::string, std::string> >();
    auto total = problems.size() ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE contest = %d AND pid in (%s) GROUP BY pid",
        atoi(argv[0].c_str()),
        res[0]["problems"].substr(1, res[0]["problems"].size() - 2).c_str()
    ) : std::vector<std::map<std::string, std::string> >();
    auto myTotal = problems.size() ? mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count, pid FROM submission WHERE uid = %d AND contest = %d AND pid in (%s) GROUP BY pid",
        userId,
        atoi(argv[0].c_str()),
        res[0]["problems"].substr(1, res[0]["problems"].size() - 2).c_str()
    ) : std::vector<std::map<std::string, std::string> >();

    auto lastSubmission = type == OI && problems.size() ? mysqli_query(
        mysql,
        "SELECT A.id, A.status, A.pid FROM submission AS A "
        "INNER JOIN ("
            "SELECT MAX(id) AS id FROM submission "
            "WHERE uid = %d AND contest = %d AND pid in (%s) "
            "GROUP BY pid"
        ") AS B ON A.id = B.id",
        userId,
        atoi(argv[0].c_str()),
        res[0]["problems"].substr(1, res[0]["problems"].size() - 2).c_str()
    ) : std::vector<std::map<std::string, std::string> >();

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["identity"] = identity;
    object["signup"] = signup;
    object["items"].resize(0);
    if (identity != Administrator) {
        if (time(NULL) < atol(res[0]["starttime"].c_str())) problems = {};
        else if (time(NULL) < atol(res[0]["starttime"].c_str()) + atol(res[0]["duration"].c_str())) {
            if (identity == Visitor) problems = {};
        }
    }
    for (int j = 0; j < problems.size(); j++) {
        Json::Value problem;
        problem["id"] = j + 1;
        problem["realId"] = atoi(problems[j]["id"].c_str());
        problem["alias"] = problems[j]["alias"];
        problem["title"] = problems[j]["title"];
        problem["difficulty"] = atoi(problems[j]["difficulty"].c_str());
        problem["tags"] = Json::arrayValue;
        Json::Value required = json_decode(problems[j]["tags"]);
        for (int k = 0; k < required.size(); k++) {
            for (int l = 0; l < tags.size(); l++) {
                if (tags[l]["id"] == required[k].asString()) {
                    Json::Value tag;
                    tag["id"] = atoi(tags[l]["id"].c_str());
                    tag["title"] = tags[l]["title"];
                    tag["type"] = atoi(tags[l]["type"].c_str());
                    problem["tags"].append(tag);
                    break;
                }
            }
        }
        problem["accepted"] = 0;
        problem["total"] = 0;
        for (int k = 0; k < accepted.size(); k++) 
            if (accepted[k]["pid"] == problems[j]["id"]) problem["accepted"] = atoi(accepted[k]["count"].c_str());
        for (int k = 0; k < total.size(); k++)
            if (total[k]["pid"] == problems[j]["id"]) problem["total"] = atoi(total[k]["count"].c_str());
        int myAcceptedCount = 0, myTotalCount = 0;
        for (int k = 0; k < myAccepted.size(); k++)
            if (myAccepted[k]["pid"] == problems[j]["id"]) myAcceptedCount = atoi(myAccepted[k]["count"].c_str());
        for (int k = 0; k < myTotal.size(); k++)
            if (myTotal[k]["pid"] == problems[j]["id"]) myTotalCount = atoi(myTotal[k]["count"].c_str());
        problem["status"] = myAcceptedCount ? 1 : myTotalCount ? 2 : 0;
        if (type == OI) {
            int cnt = 0;
            for (int k = 0; k < lastSubmission.size(); k++) 
                if (lastSubmission[k]["pid"] == problems[j]["id"]) {
                    cnt++;
                    int status = atoi(lastSubmission[k]["status"].c_str());
                    if (ended) problem["status"] = status == AC ? 1 : 2;
                    else problem["status"] = status == CE ? 2 : 1;
                    break;
                }
            if (cnt == 0) problem["status"] = 0;
        }
        problem["allowEdit"] = 
            identity == Administrator && (
                userId == atoi(problems[j]["uid"].c_str()) && hasPermission(userInfo, ProblemEdit) ||
                userId != atoi(problems[j]["uid"].c_str()) && hasPermission(userInfo, ProblemEditOthers)
            );
        problem["allowDelete"] = problem["allowEdit"];
        if (identity != Administrator) {
            if (time(NULL) < atol(res[0]["starttime"].c_str()) + atol(res[0]["duration"].c_str())) {
                if (identity == Contestant) {
                    problem["tags"].resize(0);
                    problem["difficulty"] = -1;
                    problem["accepted"] = 0;
                    problem["total"] = 0;
                    problem["allowEdit"] = false;
                    problem["allowDelete"] = false;
                }
            }
        }
        object["items"].append(problem);
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