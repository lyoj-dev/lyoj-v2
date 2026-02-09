#pragma once

#include "../../httpd.h"
#include "../../utils.cpp"

auto ProblemsData = [](client_conn conn, http_request request, param argv) {
    MYSQL mysql = quick_mysqli_connect();
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);
    const int lim = 128;

    auto res = mysqli_query(
        mysql, 
        "SELECT groups FROM problem WHERE id = %d", 
        atoi(argv[0].c_str())
    );
    if (res.size() == 0) quickSendMsg(404);

    // 权限检查
    if (argv.size() == 1 && !hasIntersection(json_decode(res[0]["groups"]), userInfo["groups"])) quickSendMsg(403);

    Json::Value config = json_decode(readFile("../problem/" + argv[0] + "/config.json"));
    std::map<int, int> subtasks = { { 0, 0 } };
    for (int i = 0; i < config["subtasks"].size(); i++)
        subtasks[config["subtasks"][i]["id"].asInt()] = i + 1;
    Json::Value datas = Json::arrayValue;
    for (int i = 0; i < config["datas"].size(); i++) {
        Json::Value data;

        std::ifstream fin("../problem/" + argv[0] + "/" + config["datas"][i]["input"].asString());
        fin.seekg(0, std::ios::end);
        int len = fin.tellg(); len = std::max(0, len);
        fin.seekg(0, std::ios::beg);
        char* ch = new char[std::min(lim, len)];
        fin.read(ch, std::min(lim, len));
        data["inputIgnored"] = argv.size() == 2 && atoi(argv[1].c_str()) ? 0 : std::max(0, len - lim);
        data["input"] = argv.size() == 2 && atoi(argv[1].c_str()) ? "" : std::string(ch, std::min(lim, len)) + (len > lim ? "..." : "");
        fin.close();

        fin.open("../problem/" + argv[0] + "/" + config["datas"][i]["output"].asString());
        fin.seekg(0, std::ios::end);
        len = fin.tellg(); len = std::max(0, len);
        fin.seekg(0, std::ios::beg);
        delete[] ch;
        ch = new char[std::min(lim, len)];
        fin.read(ch, std::min(lim, len));
        data["outputIgnored"] = argv.size() == 2 && atoi(argv[1].c_str()) ? 0 : std::max(0, len - lim);
        data["output"] = argv.size() == 2 && atoi(argv[1].c_str()) ? "" : std::string(ch, std::min(lim, len)) + (len > lim ? "..." : "");
        fin.close();
        delete[] ch;

        if (!hasPermission(userInfo, SubmissionData)) {
            data["input"] = "";
            data["inputIgnored"] = 0;
            data["output"] = "";
            data["outputIgnored"] = 0;
        }

        data["inputName"] = config["datas"][i]["input"].asString();
        data["outputName"] = config["datas"][i]["output"].asString();
        datas[subtasks[config["datas"][i]["subtask"].asInt()]].append(data);
    }
    
    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["items"] = datas;

    mysqli_close(mysql);
    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};