#include<bits/stdc++.h>
#include<sys/un.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;
#include"../shared/json.h"
#include"../shared/log.h"
#include"../shared/socket.h"
#include"../shared/type.h"
#include"../shared/mysql.h"
#include"../shared/utils.h"
int main(int argc, char** argv) {
    Json::Value judge = json_decode(readFile("./judge.json"));
    MYSQL mysql = mysqli_connect(
        judge["mysql"]["server"].asString(),
        judge["mysql"]["user"].asString(),
        judge["mysql"]["passwd"].asString(),
        judge["mysql"]["database"].asString(),
        judge["mysql"]["port"].asInt()
    );
    int id = atoi(mysqli_query(mysql, "SELECT COUNT(*) FROM submission")[0]["COUNT(*)"].c_str()) + 1;
    string code = readFile("main.cpp");
    mysqli_execute(mysql, "INSERT INTO submission VALUES (%d, 0, 1, 0, \"%s\", \"\", 0, 0, 0, 0, 0)", id, quote_encode(code).c_str());
}