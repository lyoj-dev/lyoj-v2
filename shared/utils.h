time_t clock2() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

string readFile(string path) {
    ifstream fin(path);
    if (!fin) return "";
    fin.seekg(0, ios::end);
    int len = fin.tellg();
    if (len == -1) return "";
    fin.seekg(0, ios::beg);
    char *ch = new char[len];
    fin.read(ch, len);
    fin.close();
    string res = string(ch, len);
    delete[] ch;
    return res;
}

void clearFile(string path) {
    ofstream fout(path);
    fout.close();
}

string str_replace(string from, string to, string source) {
    string result = source;
	int st = 0, wh = result.find(from.c_str(), st);
	while (wh != string::npos) {
        result.replace(wh, from.size(), to.c_str());
		st = wh + to.size();
		wh = result.find(from.c_str(), st);
	} 
    return result;
}

string quote_encode(string source) {
    return str_replace("\n", "\\n", str_replace("\"", "\\\"", str_replace("\\", "\\\\", source)));
}

string sprintf(const char* format, ...) {
    // 格式化文本
    va_list args;
    va_start(args, format);
    char buf[1024 * 1024];
    memset(buf, '\0', sizeof buf);
    int n = vsprintf(buf, format, args);
    va_end(args);
    return buf;
}