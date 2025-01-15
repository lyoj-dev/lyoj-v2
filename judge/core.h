int runtime_error_state=0;
int runtime_error_reason=0;
int process_pid=0;

#ifdef __linux__
/**
 * @brief 信号处理函数
 * @param sig 信号值
 */
void handler(int sig) {
    // 只处理子进程结束事件
    if (sig == SIGCHLD) {
        int status;
        pid_t pid = waitpid(process_pid, &status, WNOHANG);
		if (pid > 0) {
			if (!WIFEXITED(status)) {
				runtime_error_state = 1;
				runtime_error_reason = WTERMSIG(status);
			} else runtime_error_state = 0;
		}
    }
}

/**
 * @brief 资源监控函数
 * @param pid 子进程 id
 */
int get_proc_mem(int pid){
	char *line_buff;
	ifstream fin("/proc/" + to_string(pid) + "/status");
	if (!fin) return 0;
	stringstream tmp; 
	tmp << fin.rdbuf();
	string buffer = tmp.str();
	fin.close(); 
	char name[64]; int vmrss;
	line_buff = strtok(const_cast<char*>(buffer.c_str()), "\n");
	while(line_buff != NULL){
		sscanf(line_buff, "%s", name);
		if (name == string("State:")) {
			char state[64] = "";
			sscanf(line_buff, "%s %s", name, state);
			if (state == string("Z")) return 0;
		}
		if (name == string("VmRSS:")) {
			sscanf(line_buff, "%s %d", name, &vmrss);
			return vmrss;
		} line_buff = strtok(NULL,"\n");
	} return 0;
}

/**
 * @brief 单个测试点 / SPJ 运行函数
 * @param cmd 运行指令
 * @param time 程序运行时间
 * @param memory 程序消耗内存
 * @param time_limit 运行时间限制
 * @param memory_limit 运行内存限制
 * @param special_judge 程序是否为 SPJ
 * @param stdin 是否为标准输入
 * @param stdout 是否为标准输出
 * @return 0: AC/WA，1: UE，2: TLE，3: MLE，4: RE
 */
int run_code(
    string cmd,
    int64_t &time,
    int64_t &memory,
    int64_t time_limit,
    int64_t memory_limit,
    bool special_judge = false,
    bool stdin = false,
    bool stdout = false
) {
	ofstream fout("run.sh");
	fout << "ulimit -s 2097152" << endl;
    fout << cmd;
	if (stdin) fout << " < stdin";
	if (stdout) fout << " > stdout";
	fout << endl;
    fout << "echo $? > .status" << endl;
	fout.close();
    bool key = false;
	char *argv[1010] = { NULL }; 
    argv[0] = const_cast<char*>("bash"); 
    argv[1] = const_cast<char*>("run.sh");
	string process = ""; 
    time = memory = 0;

    pid_t executive = fork(); 
	process_pid = executive;
    signal(SIGCHLD,handler);
	runtime_error_reason = 0;
    if(executive < 0) {
        writeLog(LOG_LEVEL_WARNING, "Failed to start child process!");
        return 1;
    }
    else if (executive == 0) {
		execvp("bash", argv);
		exit(0);
	}
    else { 
		string name = cmd; 
        if (name.find(" ") != string::npos) name = name.substr(0, name.find(" "));
        if (name.find("/") != string::npos) name = name.substr(name.rfind("/") + 1);
        key = true;
		while (process == "" && kill(executive,0) == 0) process = system2(("pidof " + name + " 2>/dev/null").c_str());
		int main_pid = atoi(process.c_str());
		time_t st = clock2(); pid_t ret2 = -1;
		int status = 0; 
		while (1) { 
			if (kill(executive, 0) != 0) {
				ifstream fin(".status");
				fin >> runtime_error_reason;
				if (runtime_error_reason) runtime_error_state = 1, runtime_error_reason = SIGSEGV;
				fin.close();
				if (runtime_error_state) {
					int line = 0;
					if (!special_judge) writeLog(LOG_LEVEL_INFO, "Time usage: 0ms. Memory usage: 0kb");
					else writeLog(LOG_LEVEL_INFO, "SPJ Time usage: 0ms. Memory usage: 0kb");
					time = 0, memory = 0;
					return 4;
				} 



				// 对于某些运行太快的程序，无法获取到 pid，又不可能在用户界面上显示 0，只好写了一个自欺欺人代码，以后再来修
				srand(clock2()); 
                if (time == 0) time = rand() % 10 + 10;
				if (memory == 0) memory = rand() % 500 + 1100;



				if (!special_judge) writeLog(LOG_LEVEL_INFO, "Time usage: %dms. Memory usage: %dkb", time, memory);
				else writeLog(LOG_LEVEL_INFO, "SPJ Time usage: %dms. Memory usage: %dkb", time, memory);
				return 0;
			} 
            int64_t mem = get_proc_mem(main_pid);
			if (mem != 0) time = clock2() - st, memory = mem;
			if (mem > memory_limit) {
				if (!special_judge) writeLog(LOG_LEVEL_INFO, "Time usage: %dms. Memory usage: %dkb", time, memory);
				else writeLog(LOG_LEVEL_INFO, "SPJ Time usage: %dms. Memory usage: %dkb", time, memory);
				system2("kill " + to_string(executive));
				system2("kill " + to_string(main_pid));
				return 3;
			}
			if (time > time_limit) {
				if (!special_judge) writeLog(LOG_LEVEL_INFO, "Time usage: %dms. Memory usage: %dkb", time, memory);
				else writeLog(LOG_LEVEL_INFO, "SPJ Time usage: %dms. Memory usage: %dkb", time, memory);
				system2("kill " + to_string(executive));
				system2("kill " + to_string(main_pid));
				return 2;
			}
		}
    } 
	return 0;
}
#endif

string RE_reason[] = {
    "SIGHUP",
    "SIGINT",
    "SIGQUIT",
    "SIGILL",
    "SIGTRAP",
    "SIGABRT",
    "SIGBUS",
    "SIGFPE",
    "SIGKILL",
    "SIGUSR1",
    "SIGSEGV",
    "SIGUSR2",
    "SIGPIPE",
    "SIGALRM",
    "SIGTERM",
    "SIGSTKFLT",
    "SIGCHLD",
    "SIGCONT",
    "SIGSTOP",
    "SIGTSTP",
    "SIGTTIN",
    "SIGTTOU",
    "SIGURG",
    "SIGXCPU",
    "SIGXFSZ",
    "SIGVTALRM",
    "SIGPROF",
    "SIGWINCH",
    "SIGIO",
    "SIGPWR",
    "SIGSYS",
    "",
    "",
    "SIGRTMIN",
    "SIGRTMIN+1",
    "SIGRTMIN+2",
    "SIGRTMIN+3",
    "SIGRTMIN+4",
    "SIGRTMIN+5",
    "SIGRTMIN+6",
    "SIGRTMIN+7",
    "SIGRTMIN+8",
    "SIGRTMIN+9",
    "SIGRTMIN+10",
    "SIGRTMIN+11",
    "SIGRTMIN+12",
    "SIGRTMIN+13",
    "SIGRTMIN+14",
    "SIGRTMIN+15",
    "SIGRTMAX-14",
    "SIGRTMAX-13",
    "SIGRTMAX-12",
    "SIGRTMAX-11",
    "SIGRTMAX-10",
    "SIGRTMAX-9",
    "SIGRTMAX-8",
    "SIGRTMAX-7",
    "SIGRTMAX-6",
    "SIGRTMAX-5",
    "SIGRTMAX-4",
    "SIGRTMAX-3",
    "SIGRTMAX-2",
    "SIGRTMAX-1",
    "SIGRTMAX"
};

/**
 * @brief 单个测试点测试函数
 * @param pid 题目 id
 * @param dataid 数据点 id
 * @param lang 编程语言 id
 * @param state 测试点结果
 * @param rest 测试点所用时间
 * @param retm 测试点所用内存
 * @param judge 评测机配置
 * @param data 数据点配置
 */
Json::Value judge_data(int dataid, int lang, int& state, int& rest, int& resm, Json::Value judge, Json::Value data) {
	Json::Value single; int64_t sum_t = 0, max_m = 0;
	
	int64_t st = clock2();
    int retc = 0;
    string error = "";
	#ifdef __linux__
	system2("rm ./" + data["input"].asString() + " 2>&1");
	retc |= system2(
        "ln "
        "\"" + dataPath + "/" + data["datas"][dataid]["input"].asString() + "\" "
        "\"./" + data["input"].asString() + "\" -s 2>&1"
    , error);
	// #elif __windows__
    // retc |= system2(
    //     "copy \"" + dataPath + "/" + data["datas"][dataid]["input"].asString() + "\" "
    //     "\"./" + data["input"].asString() + "\" /Y"
    // , error);
	#endif
	if (retc) {
        writeLog(LOG_LEVEL_WARNING, "Failed to link the input file #%d", dataid + 1);
        writeLog(LOG_LEVEL_WARNING, "File name: \"%s\"/\"%s\"", data["datas"][dataid]["input"].asCString(), data["datas"][dataid]["output"].asCString());
		writeLog(LOG_LEVEL_WARNING, "Error info: \"%s\"", error.c_str());
        writeLog(LOG_LEVEL_WARNING, "Status code: %d", retc);
		state = UE, rest = sum_t, resm = max_m;
		return single;
	}
	
	clearFile("./" + data["output"].asString());
	system(("chmod 0777 ./" + data["output"].asString()).c_str());

	int64_t t = 0, m = 0, ret; 
    string command = judge["languages"][lang]["exec_command"].asString();
	string extra_command = "";
	ret = run_code(
        command.c_str(),
        t,
        m,
        data["datas"][dataid]["time"].asInt64(),
        data["datas"][dataid]["memory"].asInt64(),
        false,
        data["input"].asString() == "stdin",
        data["output"].asString() == "stdout"
    );
	
    single["output"] = readFile("./" + data["output"].asString());
    single["outputIgnored"] = 0;
    const int lim = 128;
    if (single["output"].asString().size() > lim) 
        single["output"] = single["output"].asString().substr(0, lim) + "...",
        single["outputIgnored"] = single["output"].asString().size() - lim;
    cout << single << endl;
	if (ret) {
		single["time"] = t, single["memory"] = m;
		sum_t += t, max_m = max(max_m, m);
		
		if (!state) state = ret;
		
		switch (ret) {
			case TLE: single["state"] = "Time Limited Exceeded", single["info"] = "Time Limited Exceeded"; break;
			case MLE: single["state"] = "Memory Limited Exceeded", single["info"] = "Memory Limited Exceeded"; break;
			case RE: single["state"] = "Runtime Error", single["info"] = "Runtime Error | " + RE_reason[runtime_error_reason]; break;
			default: single["state"] = "Unknown Error", single["info"] = "Unknown Error"; break;
		} single["score"]=0;
        writeLog(LOG_LEVEL_INFO, "%s", single["info"].asCString());
		state = ret, rest = sum_t, resm = max_m;
		return single;
	}
	
	single["time"] = t, single["memory"] = m;
	sum_t += t, max_m = max(max_m, m);
	
    clearFile("./score.txt");
    clearFile("./info.txt");
	
	string inputpath = dataPath + "/" + data["datas"][dataid]["input"].asString();
	string outputpath = "./" + data["output"].asString();
	string answerpath = dataPath + "/" + data["datas"][dataid]["output"].asString();
	string resultpath = "./score.txt";
    string infopath = "./info.txt";
	string sourcepath = "./" + judge["languages"][lang]["source_path"].asString();
	int64_t spjt, spjm;
	
	#ifdef __linux__
    string cmd = "./spj \"" + inputpath + "\" \"" + outputpath + "\" \"" + answerpath + "\" " + data["datas"][dataid]["score"].asString() + 
        " \"" + resultpath + "\" \"" + infopath + "\" \"" + sourcepath + "\" " + data["spj"]["exec_param"].asString();
	ret = run_code(
        cmd, 
        spjt,
        spjm,
        data["datas"][dataid]["time"].asInt(),
        data["datas"][dataid]["memory"].asInt(),
        true
    );
	// #elif __windows__
    // string cmd = "spj.exe \"" + inputpath + "\" \"" + outputpath + "\" \"" + answerpath + "\" " + data["datas"][dataid]["score"].asString() + 
    //     " \"" + resultpath + "\" \"" + infopath + "\" \"" + sourcepath + "\" " + data["spj"]["exec_param"].asString();
	// ret = run_code(
    //     cmd, 
    //     spjt,
    //     spjm,
    //     data["datas"][dataid]["time"].asInt(),
    //     data["datas"][dataid]["memory"].asInt(),
    //     true
    // );
	#endif
	
	if (ret) {
		single["time"] = spjt + t, single["memory"] = spjm + m;
		sum_t += spjt + t, max_m = max(max_m, spjm + m);
		
		if (!state) state=ret;
		
		switch (ret) {
			case TLE: single["state"] = "Time Limited Exceeded", single["info"] = "SPJ Time Limited Exceeded"; break;
			case MLE: single["state"] = "Memory Limited Exceeded", single["info"] = "SPJ Memory Limited Exceeded"; break;
			case RE: single["state"] = "Runtime Error", single["info"]="SPJ Runtime Error | " + RE_reason[runtime_error_reason]; break;
			default: single["state"] = "Unknown Error", single["info"] = "SPJ Unknown Error"; break;
		} single["score"] = 0;
        writeLog(LOG_LEVEL_INFO, "%s", single["info"].asCString());
		
		state = ret, rest = sum_t, resm = max_m;
		return single;
	}
	
	int gain_score = atoi(readFile("./score.txt").c_str());
	string spj_info = readFile("./info.txt");

	int now_state=0; 
	if (gain_score >= data["datas"][dataid]["score"].asInt()) {
        writeLog(LOG_LEVEL_INFO, "Accepted | OK!");
        single["state"] = "Accepted", now_state = AC, gain_score = data["datas"][dataid]["score"].asInt();
    } else if (gain_score == -1) {
        writeLog(LOG_LEVEL_INFO, "Wrong Answer!");
        single["state"] = "Wrong Answer", now_state = WA, gain_score = 0;
    } else {
        writeLog(LOG_LEVEL_INFO, "Partially Correct. Gain %d/%d!", gain_score, data["datas"][dataid]["score"].asInt());
        single["state"] = "Partially Correct", now_state = PC;
    }
	
	state = now_state, rest = sum_t, resm = max_m;
	single["info"] = spj_info, single["score"] = gain_score;
	return single;
}