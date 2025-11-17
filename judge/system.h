string system2(string cmd) {
    FILE *stream; 
    char buf[1024 * 1024]; 
    memset(buf, '\0', sizeof(buf));
#if __linux__
    stream = popen(cmd.c_str(), "r");
// #elif __windows__
//     stream = _popen(cmd.c_str(), "r");
#endif
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
    pclose(stream);
    return string(buf);
}

int system2(string cmd, string& result) {
    FILE *stream; 
    char buf[1024 * 1024]; 
    memset(buf, '\0', sizeof(buf));
#if __linux__
    stream = popen(cmd.c_str(), "r");
// #elif __windows__
//     stream = _popen(cmd.c_str(), "r");
#endif
    int k = fread(buf, sizeof(char), sizeof(buf), stream);
    result = string(buf);
    return pclose(stream);
}

void __chdir(string path) {
#if __linux__
	int retc = chdir(path.c_str());
// #elif __windows__
// 	int retc = SetCurrentDirectory(path.c_str());
#endif
}

vector<string> getFiles(string path) {
    vector<string> files;
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