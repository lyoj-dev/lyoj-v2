#pragma once

#include <cstring>
#include <dirent.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

std::string system2(std::string cmd) {
    FILE *stream; 
    char buf[1024 * 1024]; 
    std::memset(buf, '\0', sizeof(buf));
#if __linux__
    stream = popen(cmd.c_str(), "r");
// #elif __windows__
//     stream = _popen(cmd.c_str(), "r");
#endif
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
    pclose(stream);
    return std::string(buf);
}

int system2(std::string cmd, std::string& result) {
    FILE *stream; 
    char buf[1024 * 1024]; 
    memset(buf, '\0', sizeof(buf));
#if __linux__
    stream = popen(cmd.c_str(), "r");
// #elif __windows__
//     stream = _popen(cmd.c_str(), "r");
#endif
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
    result = std::string(buf);
    return pclose(stream);
}

void __chdir(std::string path) {
#if __linux__
	int retc = chdir(path.c_str());
// #elif __windows__
// 	int retc = SetCurrentDirectory(path.c_str());
#endif
}

std::vector<std::string> getFiles(std::string path) {
    std::vector<std::string> files;
    DIR *dir;
    struct dirent *ptr;
    dir = opendir(path.c_str());
    while ((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) continue;
        files.push_back(ptr->d_name);
    }
    closedir(dir);
    return files;
}