/**
 * @file main.cpp
 * @author LittleYang0531 (littleyang0531@cau.edu.cn)
 * @brief 后端服务器主文件
 * @version 1.0.0
 * @date 2025-01-13
 * 
 * @copyright Copyright (c) 2026 LittleYang0531
 * 
 */

#include"../shared/json.h"
#include"../shared/utils.h"
#include"../shared/proc.h"
#include"httpd.h"
#include"api/configurations/languages.cpp"
#include"api/configurations/spjs.cpp"
#include"api/problems/list.cpp"
#include"api/problems/listAll.cpp"
#include"api/problems/listAllTags.cpp"
#include"api/problems/details.cpp"
#include"api/problems/delete.cpp"
#include"api/problems/submit.cpp"
#include"api/problems/data.cpp"
#include"api/problems/config.cpp"
#include"api/problems/addTag.cpp"
#include"api/problems/create.cpp"
#include"api/problems/dataUpload.cpp"
#include"api/problems/dataDownload.cpp"
#include"api/problems/dataSingle.cpp"
#include"api/submissions/list.cpp"
#include"api/submissions/details.cpp"
#include"api/submissions/rejudge.cpp"
#include"api/contests/list.cpp"
#include"api/contests/details.cpp"
#include"api/contests/register.cpp"
#include"api/contests/unregister.cpp"
#include"api/contests/problems/list.cpp"
#include"api/contests/problems/listAll.cpp"
#include"api/contests/problems/details.cpp"
#include"api/contests/problems/submit.cpp"
#include"api/contests/problems/data.cpp"
#include"api/contests/submissions/list.cpp"
#include"api/contests/submissions/details.cpp"
#include"api/contests/users/listAll.cpp"
#include"api/contests/ranking.cpp"
#include"api/contests/create.cpp"
#include"api/users/login.cpp"
#include"api/users/casLogin.cpp"
#include"api/users/check.cpp"
#include"api/users/listAll.cpp"
#include"api/users/listAllGroups.cpp"
#include"api/users/details.cpp"
#include"api/users/header.cpp"
#include"api/users/banner.cpp"
#include"api/users/updateHeader.cpp"
#include"api/users/updateBanner.cpp"
#include"api/users/updatePassword.cpp"
#include"api/users/edit.cpp"
#include"api/admin/info.cpp"
#include"api/admin/problems/delete.cpp"
#include"api/admin/problems/clone.cpp"
#include"api/admin/problems/details.cpp"
#include"api/admin/problems/edit.cpp"
#include"ws/submissions/list.cpp"
#include"ws/submissions/details.cpp"

int main(int argc, char** argv) {
    std::string proc_name = argv[0];
    proc_inittitle(argv);
    proc_settitle(("lyoj-backend: master process " + proc_name).c_str());

    appConfig = json_decode(readFile("./config.json"));
    app.setopt(HTTP_ENABLE_SSL, appConfig["server.enableSSL"].asBool());
    app.setopt(HTTP_LISTEN_HOST, appConfig["server.listenHost"].asCString());
    app.setopt(HTTP_LISTEN_PORT, appConfig["server.listenPort"].asInt());
    app.setopt(HTTP_SSL_CACERT, appConfig["server.httpsCacert"].asCString());
    app.setopt(HTTP_SSL_PRIVKEY, appConfig["server.httpsPrivkey"].asCString());
    app.setopt(HTTP_MULTI_THREAD, appConfig["server.threadNumber"].asInt());
    app.setopt(HTTP_WORKER_TITLE, "lyoj-backend: http worker process");
    app.setopt(HTTP_WS_WORKER_TITLE, "lyoj-backend: websocket worker process");

    app.addRoute("/configurations/languages", ConfigurationsLanguages);
    app.addRoute("/configurations/spjs", ConfigurationsSPJs);
    app.addRoute("/problems/list", ProblemsList);
    app.addRoute("/problems/listAll", ProblemsListAll);
    app.addRoute("/problems/listAllTags", ProblemsListAllTags);
    app.addRoute("/problems/addTag", ProblemsAddTag);
    app.addRoute("/problems/%d", ProblemsDetails);
    app.addRoute("/problems/%d/delete", ProblemsDelete);
    app.addRoute("/problems/%d/submit", ProblemsSubmit);
    app.addRoute("/problems/%d/data", ProblemsData);
    app.addRoute("/problems/%d/config", ProblemsConfig);
    app.addRoute("/problems/%d/create", ProblemsCreate);
    app.addRoute("/problems/%d/data/upload", ProblemsDataUpload);
    app.addRoute("/problems/%d/data/download", ProblemsDataDownload);
    app.addRoute("/problems/%d/data/%s", ProblemsDataSingle);
    app.addRoute("/submissions/list", SubmissionsList);
    app.addRoute("/submissions/%d", SubmissionsDetails);
    app.addRoute("/submissions/%d/rejudge", SubmissionsRejudge);
    app.addRoute("/contests/list", ContestsList);
    app.addRoute("/contests/%d", ContestsDetails);
    app.addRoute("/contests/%d/register", ContestsRegister);
    app.addRoute("/contests/%d/unregister", ContestsUnregister);
    app.addRoute("/contests/%d/problems/list", ContestsProblemsList);
    app.addRoute("/contests/%d/problems/listAll", ContestsProblemsListAll);
    app.addRoute("/contests/%d/problems/%d", ContestsProblemsDetails);
    app.addRoute("/contests/%d/problems/%d/submit", ContestsProblemsSubmit);
    app.addRoute("/contests/%d/problems/%d/data", ContestsProblemsData);
    app.addRoute("/contests/%d/submissions/list", ContestsSubmissionsList);
    app.addRoute("/contests/%d/submissions/%d", ContestsSubmissionsDetails);
    app.addRoute("/contests/%d/users/listAll", ContestsUsersListAll);
    app.addRoute("/contests/%d/ranking", ContestsRanking);
    app.addRoute("/contests/%d/create", ContestsCreate);
    app.addRoute("/users/login", UsersLogin);
    app.addRoute("/users/casLogin", UsersCasLogin);
    app.addRoute("/users/check", UsersCheck);
    app.addRoute("/users/listAll", UsersListAll);
    app.addRoute("/users/listAllGroups", UsersListAllGroups);
    app.addRoute("/users/%d", UsersDetails);
    app.addRoute("/users/%d/header", UsersHeader);
    app.addRoute("/users/%d/banner", UsersBanner);
    app.addRoute("/users/%d/header/update", UsersUpdateHeader);
    app.addRoute("/users/%d/banner/update", UsersUpdateBanner);
    app.addRoute("/users/%d/edit", UsersEdit);
    app.addRoute("/users/%d/password/update", UsersUpdatePassword);
    app.addRoute("/admin/info", AdminInfo);
    app.addRoute("/admin/problems/delete", AdminProblemsDelete);
    app.addRoute("/admin/problems/clone", AdminProblemsClone);
    app.addRoute("/admin/problems/details", AdminProblemsDetails);
    app.addRoute("/admin/problems/edit", AdminProblemsEdit);

    app.ws_addRoute("/submissions/list", WSSubmissionsList);
    app.ws_addRoute("/submissions/%d", WSSubmissionsDetails);

    __default_response["Access-Control-Allow-Credentials"] = "true";
    __default_response["Access-Control-Allow-Headers"] = "*";
    __default_response["Access-Control-Allow-Methods"] = "*";
    app.run();
}