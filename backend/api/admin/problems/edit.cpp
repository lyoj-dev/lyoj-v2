#include "../../../httpd.h"
#include "../../../utils.cpp"

auto AdminProblemsEdit = [](client_conn conn, http_request request, param argv) {
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

    std::string setString = "";
    if ($_POST["item"].isMember("alias")) setString += "alias = \"" + $_POST["item"]["alias"].asString() + "\", ";
    if ($_POST["item"].isMember("title")) setString += "title = \"" + $_POST["item"]["title"].asString() + "\", ";
    if ($_POST["item"].isMember("bg")) setString += "bg = \"" + $_POST["item"]["bg"].asString() + "\", ";
    if ($_POST["item"].isMember("descrip")) setString += "descrip = \"" + $_POST["item"]["descrip"].asString() + "\", ";
    if ($_POST["item"].isMember("input")) setString += "input = \"" + $_POST["item"]["input"].asString() + "\", ";
    if ($_POST["item"].isMember("output")) setString += "output = \"" + $_POST["item"]["output"].asString() + "\", ";
    if ($_POST["item"].isMember("hint")) setString += "hint = \"" + $_POST["item"]["hint"].asString() + "\", ";
    if ($_POST["item"].isMember("tags")) setString += "tags = \"" + json_encode($_POST["item"]["tags"]) + "\", ";
    if ($_POST["item"].isMember("groups")) setString += "groups = \"" + json_encode($_POST["item"]["groups"]) + "\", ";
    if ($_POST["item"].isMember("langs")) setString += "langs = \"" + json_encode($_POST["item"]["langs"]) + "\", ";

    if (setString != "") {
        setString = setString.substr(0, setString.size() - 2);
        mysqli_execute(mysql, "UPDATE problem SET %s WHERE id in %s", setString.c_str(), ids.c_str());
    }

    if ($_POST["item"].isMember("inputFile") || $_POST["item"].isMember("outputFile"))
        for (int i = 0; i < $_POST["ids"].size(); i++) {
            int id = $_POST["ids"][i].asInt();
            Json::Value config = json_decode(readFile("../problem/" + std::to_string(id) + "/config.json"));
            if ($_POST["item"].isMember("inputFile")) config["input"] = $_POST["item"]["inputFile"].asString();
            if ($_POST["item"].isMember("outputFile")) config["output"] = $_POST["item"]["outputFile"].asString();
            std::ofstream fout("../problem/" + std::to_string(id) + "/config.json");
            fout << json_encode(config);
            fout.close();
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