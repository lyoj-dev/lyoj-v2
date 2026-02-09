#include "../../httpd.h"
#include "../../utils.cpp"

auto ContestsRanking = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    int identity = getContestIdentity(userId, atoi(argv[0].c_str()));
    bool signup = checkSignUp(userId, atoi(argv[0].c_str()));
    auto $_GET = getParam(request);

    auto contest = mysqli_query(
        mysql,
        "SELECT * FROM contest WHERE id = %d",
        atoi(argv[0].c_str())
    );
    if (contest.size() == 0) quickSendMsg(404);
    int type = atoi(contest[0]["type"].c_str());
    bool ended = time(NULL) > atol(contest[0]["starttime"].c_str()) + atol(contest[0]["duration"].c_str());
    int page = $_GET.find("page") == $_GET.end() ? 1 : atoi($_GET["page"].c_str());
    int pageCount = (atoi(mysqli_query(
        mysql,
        "SELECT COUNT(*) AS count FROM contest_ranking WHERE id = %d",
        atoi(argv[0].c_str())
    )[0]["count"].c_str()) + 9) / 10;
    if (page < 1) page = 1;
    if (page > pageCount) page = pageCount;

    auto res = pageCount ? mysqli_query(
        mysql,
        "SELECT * FROM contest_ranking WHERE id = %d ORDER BY score DESC, %s ASC LIMIT 10 OFFSET %d",
        atoi(argv[0].c_str()),
        type == ACM ? "time2 + penalty * 20 * 60" : "time",
        (page - 1) * 10
    ) : std::vector<argvar>();
    auto scoresRes = mysqli_query(
        mysql,
        "SELECT score FROM contest_ranking WHERE id = %d",
        atoi(argv[0].c_str())
    );
    int *scores = new int[scoresRes.size()];
    for (int i = 0; i < scoresRes.size(); i++) scores[i] = atoi(scoresRes[i]["score"].c_str());
    std::sort(scores, scores + scoresRes.size());
    auto my = mysqli_query(
        mysql,
        "SELECT * FROM contest_ranking WHERE id = %d AND uid = %d",
        atoi(argv[0].c_str()),
        userId
    );

    std::string userList = "";
    for (int i = 0; i < res.size(); i++) userList += (i ? "," : "") + res[i]["uid"];
    if (my.size()) userList += (userList == "" ? "" : ",") + std::to_string(userId);
    auto users = userList == "" ? std::vector<argvar>() : mysqli_query(
        mysql,
        "SELECT id, title FROM user WHERE id in (%s)",
        userList.c_str()
    );

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["identity"] = identity;
    object["signup"] = signup;
    object["pageCount"] = pageCount;
    object["items"].resize(0);
    for (int i = 0; i < res.size(); i++) {
        Json::Value single;
        single["id"] = type == ACM ? 
            i + 1 + (page - 1) * 10 : 
            scoresRes.size() - (std::upper_bound(scores, scores + scoresRes.size(), atoi(res[i]["score"].c_str())) - scores) + 1;
        single["uid"] = atoi(res[i]["uid"].c_str());
        single["user"] = "";
        for (int j = 0; j < users.size(); j++) {
            if (users[j]["id"] == res[i]["uid"]) {
                single["user"] = users[j]["title"];
                break;
            }
        }
        single["score"] = atoi(res[i]["score"].c_str());
        single["time"] = atoi(res[i]["time"].c_str());
        single["time2"] = atoi(res[i]["time2"].c_str());
        single["penalty"] = atoi(res[i]["penalty"].c_str());
        single["info"] = json_decode(res[i]["info"]);
        object["items"].append(single);
    }
    if (my.size()) {
        object["item"]["id"] = type == ACM ? 
            atoi(mysqli_query(
                mysql,
                "SELECT COUNT(*) AS count FROM contest_ranking WHERE id = %d AND (score > %d OR score = %d AND time2 + penalty * 20 * 60 < %d)", 
                atoi(argv[0].c_str()), 
                atoi(my[0]["score"].c_str()),
                atoi(my[0]["score"].c_str()),
                atoi(my[0]["time2"].c_str()) + atoi(my[0]["penalty"].c_str()) * 20 * 60
            )[0]["count"].c_str()) + 1 :
            scoresRes.size() - (std::upper_bound(scores, scores + scoresRes.size(), atoi(my[0]["score"].c_str())) - scores) + 1;
        object["item"]["uid"] = userId;
        object["item"]["user"] = "";
        for (int j = 0; j < users.size(); j++) {
            if (atoi(users[j]["id"].c_str()) == userId) {
                object["item"]["user"] = users[j]["title"];
                break;
            }
        }
        object["item"]["score"] = atoi(my[0]["score"].c_str());
        object["item"]["time"] = atoi(my[0]["time"].c_str());
        object["item"]["time2"] = atoi(my[0]["time2"].c_str());
        object["item"]["penalty"] = atoi(my[0]["penalty"].c_str());
        object["item"]["info"] = json_decode(my[0]["info"]);
    }

    if (type == OI && !ended && identity != Administrator) {
        object["items"].resize(0);
        if (my.size()) {
            object["item"]["score"] = 0;
            object["item"]["time"] = 0;
            object["item"]["time2"] = 0;
            object["item"]["penalty"] = 0;
            object["item"]["id"] = 1;
            for (int i = 0; i < object["item"]["info"].size(); i++) {
                object["item"]["info"][i]["score"] = 0;
                object["item"]["info"][i]["time"] = 0;
                object["item"]["info"][i]["time2"] = 0;
                object["item"]["info"][i]["penalty"] = 0;
                object["item"]["info"][i]["sid"] = 0;
            }    
        }
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