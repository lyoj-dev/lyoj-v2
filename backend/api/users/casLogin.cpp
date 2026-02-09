#include "../../httpd.h"
#include "../../utils.cpp"
#include "../../../shared/socket.h"
#include "login.cpp"
#include<netdb.h>

std::string getContent(std::string source, std::string prefix, std::string suffix, int pt = 0) {
    int st = source.find(prefix, pt) + prefix.size();
    int ed = source.find(suffix, st);
    return source.substr(st, ed - st);
}

std::vector<std::pair<std::string, std::string> > htmlEntities = {
    {"&", "&amp;"},
    {" ", "&nbsp;"},
    {">", "&gt;"},
    {"<", "&lt;"},
    {"\"", "&quot;"},
    {"'", "&apos;"}
};
std::string htmldecode(std::string s) {
    for (int i = 0; i < 256; i++) htmlEntities.push_back({std::string(1, i), "&#" + std::to_string(i) + ";"});
    for (auto entity : htmlEntities) s = str_replace(entity.second, entity.first, s);
    return s;
}

auto UsersCasLogin = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (userId != 0) quickSendMsgWithoutMySQL(400);
    auto $_GET = getParam(request);
    if ($_GET.find("ticket") == $_GET.end()) quickSendMsgWithoutMySQL(400);

    std::string ticket = $_GET["ticket"];

    std::string host = appConfig["cas"]["host"].asString();
    std::string ip = [&](){
        auto res = gethostbyname(host.c_str());
        return std::string(inet_ntoa(*(struct in_addr*)res->h_addr_list[0]));
    }();
    Client client(ip, appConfig["cas"]["port"].asInt());
    Connection conn2 = client.connect();
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL* ssl;
    if (appConfig["cas"]["sslIgnoreCert"].asBool()) SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    if (appConfig["cas"]["ssl"].asBool()) {
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, conn2.conn);
        if (SSL_connect(ssl) == -1) quickSendMsgWithoutMySQL(500);
    }
    std::stringstream ss;
    ss << "GET " << appConfig["cas"]["prefix"].asString() << "?ticket=" << ticket << "&service=" << urlencode(appConfig["cas"]["service"].asString()) << " HTTP/1.1\r\n";
    ss << "User-Agent: LittleYang OnlineJudge Backend Service\r\n";
    ss << "Host: " << appConfig["cas"]["host"].asString() << ":" << appConfig["cas"]["port"].asInt() << "\r\n";
    ss << "Connection: keep-alive\r\n\r\n";
    if (!appConfig["cas"]["ssl"].asBool()) send(conn2.conn, ss.str().c_str(), ss.str().size(), 0);
    else SSL_write(ssl, ss.str().c_str(), ss.str().size());
    char* ch = new char[1024 * 1024];
    int len = 0;
    if (!appConfig["cas"]["ssl"].asBool()) len = recv(conn2.conn, ch, 1024 * 1024, 0);
    else len = SSL_read(ssl, ch, 1024 * 1024);
    std::string r = std::string(ch, len);
    std::string data = r.substr(r.find("\r\n\r\n") + 4);
    delete[] ch;

    Json::Value object;
    object["code"] = 200;
    object["success"] = true;
    object["msg"] = "OK";
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    if (data.find("cas:authenticationFailure") != std::string::npos) {
        object["success"] = false;
        object["msg"] = "Authentication Failure";
        int pt = data.find("authenticationFailure") + std::string("authenticationFailure").size();
        object["failureCode"] = getContent(data, "code=\"", "\"", pt);
        object["failureMsg"] = htmldecode(getContent(data, ">", "<", pt));
    } else if (data.find("cas:authenticationSuccess") != std::string::npos) {
        object["ID_NUMBER"] = getContent(data, "<cas:ID_NUMBER>", "</cas:ID_NUMBER>"); // 学号
        object["ID_TYPE"] = atoi(getContent(data, "<cas:ID_TYPE>", "</cas:ID_TYPE>").c_str()); // 学生类型
        object["UNIT_ID"] = atoi(getContent(data, "<cas:UNIT_ID>", "</cas:UNIT_ID>").c_str()); // 学院ID
        object["UNIT_NAME"] = getContent(data, "<cas:UNIT_NAME>", "</cas:UNIT_NAME>"); // 学院名称
        object["USER_SEX"] = atoi(getContent(data, "<cas:USER_SEX>", "</cas:USER_SEX>").c_str()); // 用户性别
        object["USER_FIRST_LOGIN"] = getContent(data, "<cas:USER_FIRST_LOGIN>", "</cas:USER_FIRST_LOGIN>"); 
        object["USER_ID"] = getContent(data, "<cas:USER_ID>", "</cas:USER_ID>");
        object["USER_NAME"] = getContent(data, "<cas:USER_NAME>", "</cas:USER_NAME>"); // 姓名
        object["TYPE_NAME"] = getContent(data, "<cas:TYPE_NAME>", "</cas:TYPE_NAME>"); // 学生类型名称
        std::string schoolId = object["ID_NUMBER"].asString();

        MYSQL mysql = quick_mysqli_connect();
        auto res = mysqli_query(mysql, "SELECT * FROM user WHERE idNumber = \"%s\"", schoolId.c_str());
        int uid = 0;
        if (res.size() == 0) {
            uid = atoi(mysqli_query(mysql, "SELECT MAX(id) AS count FROM user")[0]["count"].c_str()) + 1;
            std::string title = object["USER_NAME"].asString();
            std::string name = object["USER_NAME"].asString();
            std::string idNumber = object["ID_NUMBER"].asString();
            int idType = object["ID_TYPE"].asInt();
            std::string typeName = object["TYPE_NAME"].asString();
            int unitId = object["UNIT_ID"].asInt();
            std::string unitName = object["UNIT_NAME"].asString();
            int sex = object["USER_SEX"].asInt();
            std::string password = "";
            time_t createTime = time(NULL);
            int rating = 0;
            std::string groups = "[1]";
            std::string info = "";
            mysqli_execute(
                mysql,
                "INSERT INTO user VALUES (%d, \"%s\", \"%s\", \"%s\", %d, \"%s\", %d, \"%s\", %d, \"%s\", %d, %d, \"%s\", \"%s\")",
                uid,
                quote_encode(title).c_str(),
                quote_encode(name).c_str(),
                quote_encode(idNumber).c_str(),
                idType,
                quote_encode(typeName).c_str(),
                unitId,
                quote_encode(unitName).c_str(),
                sex,
                quote_encode(password).c_str(),
                createTime,
                rating,
                quote_encode(groups).c_str(),
                quote_encode(info).c_str()
            );
            system(("cp -r ./data/users/0 ./data/users/" + std::to_string(uid)).c_str());
        } else uid = atoi(res[0]["id"].c_str());
        std::string session = generateSession();
        mysqli_execute(
            mysql,
            "INSERT INTO session VALUES (%d, \"%s\", %ld)",
            uid,
            session.c_str(),
            time(NULL)
        );
        object["session"] = session;
        mysqli_close(mysql);
    } else {
        object["success"] = false;
        object["msg"] = "Bad Request";
    }

    std::string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = std::to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};