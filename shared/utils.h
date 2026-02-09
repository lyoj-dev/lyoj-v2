#pragma once

#include <chrono>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>
time_t clock2() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::string readFile(std::string path) {
    std::ifstream fin(path);
    if (!fin) return "";
    fin.seekg(0, std::ios::end);
    int len = fin.tellg();
    if (len == -1) return "";
    fin.seekg(0, std::ios::beg);
    char *ch = new char[len];
    fin.read(ch, len);
    fin.close();
    std::string res = std::string(ch, len);
    delete[] ch;
    return res;
}

void clearFile(std::string path) {
    std::ofstream fout(path);
    fout.close();
}

std::string str_replace(std::string from, std::string to, std::string source) {
    std::string result = source;
	int st = 0, wh = result.find(from.c_str(), st);
	while (wh != std::string::npos) {
        result.replace(wh, from.size(), to.c_str());
		st = wh + to.size();
		wh = result.find(from.c_str(), st);
	} 
    return result;
}

std::string quote_encode(std::string source) {
    return str_replace("\n", "\\n", str_replace("\"", "\\\"", str_replace("\\", "\\\\", source)));
}

std::string sprintf(const char* format, ...) {
    // 格式化文本
    va_list args;
    va_start(args, format);
    char buf[1024 * 1024];
    memset(buf, '\0', sizeof buf);
    int n = vsprintf(buf, format, args);
    va_end(args);
    return buf;
}