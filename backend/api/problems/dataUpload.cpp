#include<dirent.h>

vector<string> getAllFiles(string path) {
    vector<string> res;
    DIR *dir;
    struct dirent *ptr;
    if ((dir = opendir(path.c_str())) == NULL) return res;
    while ((ptr = readdir(dir)) != NULL) {
        if (ptr->d_name[0] == '.') continue;
        res.push_back(ptr->d_name);
    }
    closedir(dir);
    return res;
}

bool compareNatural(string a, string b) {
    if (a == "") return true;
    if (b == "") return false;
    if (isdigit(a[0]) && !isdigit(b[0])) return true;
    if (!isdigit(a[0]) && isdigit(b[0])) return false;
    if (!isdigit(a[0]) && !isdigit(b[0])) {
        if (toupper(a[0]) == toupper(b[0])) return compareNatural(a.substr(1), b.substr(1));
        return toupper(a[0]) < toupper(b[0]);
    }
    stringstream ss1(a), ss2(b);
    int val1, val2;
    ss1 >> val1; ss2 >> val2;
    if (val1 != val2) return val1 < val2;
    string rest1 = a.substr(ss1.tellg()), rest2 = b.substr(ss2.tellg());
    return compareNatural(rest1, rest2);
}

auto ProblemsDataUpload = [](client_conn conn, http_request request, param argv) {
    int userId = getUserId(request);
    auto userInfo = getUserInfo(userId);

    if (request.method != "POST") quickSendMsgWithoutMySQL(405);

    MYSQL mysql = quick_mysqli_connect();
    auto res = mysqli_query(mysql, "SELECT uid FROM problem WHERE id = " + argv[0]);
    if (res.size() == 0) quickSendMsg(404);
    if (atoi(res[0]["uid"].c_str()) == userId && !hasPermission(userInfo, ProblemEdit)) quickSendMsg(403);
    if (atoi(res[0]["uid"].c_str()) != userId && !hasPermission(userInfo, ProblemEditOthers)) quickSendMsg(403);
    mysqli_close(mysql);

    auto data = request.postdata;
    data = base64_decode(data);
    int id = atoi(argv[0].c_str());

    system(("find ../problem/" + to_string(id) + "/* | grep -v config.json | xargs rm > /dev/null 2>&1").c_str());
    ofstream fout("../problem/" + to_string(id) + "/data.zip", ios::binary);
    fout.write(data.c_str(), data.size());
    fout.close();

    system(("cd ../problem/" + to_string(id) + " && unzip -n data.zip > /dev/null 2>&1 && rm data.zip").c_str());

    auto files = getAllFiles("../problem/" + to_string(id));
    map<string, vector<string> > numbers;
    for (auto file : files) {
        string name = file.find(".") != string::npos ? file.substr(0, file.rfind(".")) : file;
        numbers[name].push_back(file);
    }
    vector<pair<string, string> > datas;
    for (auto item : numbers) {
        string hasInput = "", hasOutput = "";
        string name = item.first;
        name = name.find(".") != string::npos ? name.substr(0, name.rfind(".")) : name;
        for (auto file : item.second) {
            if (file.find(".in") != string::npos) hasInput = file;
            if (file.find(".out") != string::npos || file.find(".ans") != string::npos) hasOutput = file;
        }
        if (hasInput != "" && hasOutput != "") datas.push_back({hasInput, hasOutput});
    }
    sort(datas.begin(), datas.end(), [](auto a, auto b) {
        string name1 = a.first, name2 = b.first;
        name1 = name1.find(".") != string::npos ? name1.substr(0, name1.rfind(".")) : name1;
        name2 = name2.find(".") != string::npos ? name2.substr(0, name2.rfind(".")) : name2;
        return compareNatural(name1, name2);
    });

    Json::Value configs = json_decode(readFile("../problem/" + to_string(id) + "/config.json"));
    configs["datas"].resize(0);
    for (int i = 0; i < datas.size(); i++) {
        configs["datas"][i]["input"] = datas[i].first;
        configs["datas"][i]["output"] = datas[i].second;
        configs["datas"][i]["time"] = 1000;
        configs["datas"][i]["memory"] = 128 * 1024;
        configs["datas"][i]["score"] = 100 / datas.size() + (i >= datas.size() - 100 % datas.size());
        configs["datas"][i]["subtask"] = 0;
    }
    fout.open("../problem/" + to_string(id) + "/config.json");
    fout << json_encode(configs);
    fout.close();

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["loginAs"] = userId;
    object["loginInfo"] = userInfo;
    object["datas"].resize(0);
    for (int i = 0; i < datas.size(); i++) {
        object["datas"][i]["input"] = datas[i].first;
        object["datas"][i]["output"] = datas[i].second;
    }

    string responseBody = json_encode(object);
    auto response = __api_default_response;
    response["Access-Control-Allow-Origin"] = request.argv["origin"];
    response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, response);
    send(conn, responseBody);
    exitRequest(conn);
};