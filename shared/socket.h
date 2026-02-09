#pragma once

#include "log.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <sys/stat.h>
#include <sys/un.h>
const std::string errorKey = [](){
    std::string key = "";
    for (int i = 0; i < 256; i++) key += char(rand() % 26 + 'a');
    return key;
}();

struct Connection {
    bool isUnix;
    int conn;
    sockaddr_in addr;
    sockaddr_un sock;

    bool send(std::string msg) {
        int64_t len = msg.size();
        for (int i = 0; i < 8; i++) msg = char(len & ((1 << 8) - 1)) + msg, len >>= 8;

        int64_t send = 0;
        while(true) {
            int64_t s = ::send(conn, msg.substr(send).c_str(), msg.size() - send, 0);
            if (s == -1) {
                writeLog(LOG_LEVEL_ERROR, "Failed to send message! Errno: %d", &errno);
                return false;
            }
            send += s;
            if (send == msg.size()) return true;
        }
    }

    std::string recv() {
        char *len = new char[8];
        int s = ::recv(conn, len, 8, 0);
        if (s != 8) {
            delete[] len;
            return errorKey;
        }

        int64_t msgLen = 0;
        for (int i = 0; i < 8; i++) msgLen <<= 8, msgLen += (unsigned char)len[i];
        delete[] len;

        char *msg = new char[msgLen];
        int64_t recv = 0;
        while(true) {
            int64_t s = ::recv(conn, msg + recv, msgLen - recv, 0);
            if (s == -1) {
                writeLog(LOG_LEVEL_ERROR, "Failed to recv message! Error: %d", &errno);
                return errorKey;
            }
            recv += s;
            if (recv == msgLen) break;
        }

        std::string result = std::string(msg, msgLen);
        delete[] msg;
        return result;
    }
};

struct Server {
    bool isUnix;
    int sock, conn;
    sockaddr_in clientAddr;
    sockaddr_un clientSock;

    Server(std::string host, int port): isUnix(false) {
        sockaddr_in serverAddr;

        #ifdef __linux__
        bzero(&serverAddr, sizeof(serverAddr));
        // #elif __windows__
        // WORD w_req = MAKEWORD(2, 2);
        // WSADATA wsadata; int err;
        // err = WSAStartup(w_req, &wsadata);
        // if (err != 0) {
        //     writeLog(LOG_LEVEL_ERROR, "Failed to initialize SOCKET!");
        //     exit(3);
        // }
        // if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        //     writeLog(LOG_LEVEL_ERROR, "SOCKET version is not correct!");
        //     exit(3);
        // }
        #endif

        // 创建套接字
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
        serverAddr.sin_port = htons(port);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            
        }

        // 绑定
        #ifdef __linux__
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        #endif
        int ret = bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (ret < 0) {
            
        }

        // 设置监听
        ret = listen(sock,1);
        if (ret < 0) {
            
        }
    }
    
    Server(std::string sockPath): isUnix(true) {
        // 一定要先把上一个 sock 文件删了
        unlink(sockPath.c_str());

        sockaddr_un serverAddr;

        #ifdef __linux__
        bzero(&serverAddr, sizeof(serverAddr));
        // #elif __windows__
        // WORD w_req = MAKEWORD(2, 2);
        // WSADATA wsadata; int err;
        // err = WSAStartup(w_req, &wsadata);
        // if (err != 0) {
        //     writeLog(LOG_LEVEL_ERROR, "Failed to initialize SOCKET!");
        //     exit(3);
        // }
        // if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
        //     writeLog(LOG_LEVEL_ERROR, "SOCKET version is not correct!");
        //     exit(3);
        // }
        #endif

        // 创建套接字
        serverAddr.sun_family = AF_LOCAL;
        strcpy(serverAddr.sun_path, sockPath.c_str());
        sock = socket(PF_LOCAL, SOCK_STREAM, 0);
        if (sock < 0) {
            
        }

        // 绑定
        int ret = bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (ret < 0) {
            
        }

        // 设置监听
        chmod(sockPath.c_str(), 0777);
        ret = listen(sock,1);
        if (ret < 0) {
            
        }
    }

    Connection accept() {
        socklen_t clientAddrLength = isUnix ? sizeof clientSock : sizeof clientAddr;
        if (isUnix) conn = ::accept(sock, (struct sockaddr*)&clientSock, &clientAddrLength);
        else conn = ::accept(sock, (struct sockaddr*)&clientAddr, &clientAddrLength);
        if (conn < 0) {

        }
        return Connection({
            isUnix: isUnix,
            conn: conn,
            addr: clientAddr,
            sock: clientSock
        });
    }
};

struct Client {
    bool isUnix;
    int sock, conn;
    sockaddr_in serverAddr;
    sockaddr_un serverSock;

    Client(std::string host, int port): isUnix(false) {
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
        serverAddr.sin_port = htons(port);
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {

        }
    }

    Client(std::string sockPath): isUnix(true) {
        serverSock.sun_family = AF_LOCAL;
        strcpy(serverSock.sun_path, sockPath.c_str());
        sock = socket(PF_LOCAL, SOCK_STREAM, 0);
        if (sock < 0) {

        }
    }

    Connection connect() {
        if (isUnix) conn = ::connect(sock, (struct sockaddr*)&serverSock, sizeof(serverSock));
        else conn = ::connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (conn < 0) {

        }
        return Connection({
            isUnix: isUnix,
            conn: sock,
            addr: serverAddr,
            sock: serverSock
        });
    }
};