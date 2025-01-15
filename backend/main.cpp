/**
 * @file main.cpp
 * @author LittleYang0531 (littleyang0531@cau.edu.cn)
 * @brief 后端服务器主文件
 * @version 1.0.0
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2025 LittleYang0531
 * 
 */

#include<bits/stdc++.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;
#include"../shared/json.h"
#include"../shared/log.h"
#include"../shared/mysql.h"
#include"../shared/socket.h"
#include"../shared/utils.h"
#include"../shared/type.h"
#include"encrypt.h"
#include"httpd.h"

Json::Value appConfig;
MYSQL quick_mysqli_connect() {
    return mysqli_connect(
        appConfig["mysql"]["server"].asString(),
        appConfig["mysql"]["user"].asString(),
        appConfig["mysql"]["passwd"].asString(),
        appConfig["mysql"]["database"].asString(),
        appConfig["mysql"]["port"].asInt()
    );
}
#define quickSendMsg(code) { \
    Json::Value msg; \
    msg["code"] = code; \
    msg["msg"] = http_code[code]; \
    string responseBody = json_encode(msg); \
    __api_default_response["Content-Length"] = to_string(responseBody.size()); \
    putRequest(conn, 200, __api_default_response); \
    send(conn, responseBody); \
    exitRequest(conn); \
}

#include"api/configurations/languages.cpp"
#include"api/problems/list.cpp"
#include"api/problems/listAll.cpp"
#include"api/problems/listAllTags.cpp"
#include"api/problems/details.cpp"
#include"api/problems/submit.cpp"
#include"api/problems/data.cpp"
#include"api/submissions/list.cpp"
#include"api/submissions/details.cpp"
#include"ws/submissions/list.cpp"
#include"ws/submissions/details.cpp"

void* judge_thread(void* argv) {
    system("cd ../judge && ./judge");
    return NULL;
}

int main() {
    pthread_t pt;
    // pthread_create(&pt, NULL, judge_thread, NULL);

    appConfig = json_decode(readFile("./config.json"));
    app.setopt(HTTP_ENABLE_SSL, appConfig["server.enableSSL"].asBool());
    app.setopt(HTTP_LISTEN_HOST, appConfig["server.listenHost"].asString().c_str());
    app.setopt(HTTP_LISTEN_PORT, appConfig["server.listenPort"].asInt());
    app.setopt(HTTP_SSL_CACERT, appConfig["server.httpsCacert"].asString().c_str());
    app.setopt(HTTP_SSL_PRIVKEY, appConfig["server.httpsPrivkey"].asString().c_str());
    app.setopt(HTTP_MULTI_THREAD, appConfig["server.threadNumber"].asInt());

    app.addRoute("/configurations/languages", ConfigurationsLanguages);
    app.addRoute("/problems/list", ProblemsList);
    app.addRoute("/problems/listAll", ProblemsListAll);
    app.addRoute("/problems/listAllTags", ProblemsListAllTags);
    app.addRoute("/problems/%d", ProblemsDetails);
    app.addRoute("/problems/%d/submit", ProblemsSubmit);
    app.addRoute("/problems/%d/data", ProblemsData);
    // app.addRoute("/problems/%d/data/%s", ProblemsData);
    app.addRoute("/submissions/list", SubmissionsList);
    app.addRoute("/submissions/%d", SubmissionsDetails);

    app.ws_addRoute("/submissions/list", WSSubmissionsList);
    app.ws_addRoute("/submissions/%d", WSSubmissionsDetails);

    app.run();
}