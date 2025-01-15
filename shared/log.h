string logLevels[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "ASSERT"
};

enum LOG_LEVEL {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_ASSERT
};

enum LOG_TARGET {
    LOG_TARGET_FILE = 1 << 0,
    LOG_TARGET_CONSOLE = 1 << 1
};

class LogHelper {
    public:

    LOG_LEVEL logLevelId = LOG_LEVEL_INFO;
    LOG_TARGET target = LOG_TARGET_CONSOLE;
    string targetPath = "log.txt";
    pthread_mutex_t mutex;

    LogHelper() {
        pthread_mutex_init(&mutex, NULL);
    }

    void write(LOG_LEVEL levelId, string codeFile, int line, const char* format, ...) {
        pthread_mutex_lock(&mutex);
        if (levelId >= logLevelId) {
            // 格式化文本
            va_list args;
            va_start(args, format);
            char buf[8192]; 
            memset(buf, '\0', sizeof buf);
            int n = vsprintf(buf, format, args);
            va_end(args);

            // 格式化时间
            char st[1024];
            time_t t = time(NULL);
            strftime(st, sizeof st, "%Y-%m-%d %H:%M:%S", localtime(&t));

            // 获取线程id
            int tid = -1;
            #if __linux__
            tid = syscall(__NR_gettid);
            // #elif __windows__
            // tid = GetCurrentThreadId();
            #endif

            if (target & LOG_TARGET_FILE) {
                ofstream fout(targetPath, ios::app);
                fout << "[" << st << "] "
                     << "[" << logLevels[levelId] << "] "
                     << "[tid:" << tid << "] "
                     << "[" << codeFile << ":" << line << "] " << buf << endl;
                fout.close();
            }
            if (target & LOG_TARGET_CONSOLE) {
                cerr << "[" << st << "] "
                     << "[" << logLevels[levelId] << "] "
                     << "[tid:" << tid << "] "
                     << "[" << codeFile << ":" << line << "] " << buf << endl;
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}Log;

#define writeLog(loglevel, ...) Log.write(loglevel, __FILE__, __LINE__, __VA_ARGS__)