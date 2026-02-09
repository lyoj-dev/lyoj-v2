#include "../../httpd.h"
#include "../../utils.cpp"

auto UsersUpdateHeader = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);
    if (userId != atoi(argv[0].c_str()) && !hasPermission(userInfo, UserEditOthers)) quickSendMsgWithoutMySQL(403);

    auto data = request.postdata;
    data = base64_decode(data);
    std::ofstream fout("./data/users/" + argv[0] + "/header.jpg", std::ios::binary);
    fout.write(data.c_str(), data.size());
    fout.close();

    quickSendMsgWithoutMySQL(200);
};