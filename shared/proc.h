#pragma once

#include <cassert>
#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <openssl/sha.h>
#include <semaphore.h>
#include <set>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <vector>

template<typename T>
T* __my_builtin_createSharedMemory(std::string typeName, std::size_t len) {
    void *addr = mmap(NULL, len * sizeof(T), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    // std::cout << "Allocated shared memory, type = " << typeName << ", len = " << len << ", size = " << len * sizeof(T) << std::endl;
    if (addr == MAP_FAILED) return NULL;
    return (T*)addr;
}

#define createSharedMemory(T) __my_builtin_createSharedMemory<T>(#T, 1)
#define createSharedMemoryArray(T, len) __my_builtin_createSharedMemory<T>(#T, len)

template <typename T>
struct SharedMemoryAllocator {
    using value_type = T;

    SharedMemoryAllocator() = default;
    ~SharedMemoryAllocator() = default;

    template <typename U>
    SharedMemoryAllocator(const SharedMemoryAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        T* addr = createSharedMemoryArray(T, n);
        // if (addr == NULL) std::cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!1=====================" << std::endl;
        return addr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        // std::cout << "Deallocated shared memory, type = T, len = " << n << ", size = " << n * sizeof(T) << std::endl;
        munmap(p, n * sizeof(T));
    }
};

template<typename Tp>
using SharedVector = std::vector<Tp, SharedMemoryAllocator<Tp> >;
template<typename Key, typename Compare = std::less<Key> >
using SharedSet = std::set<Key, Compare, SharedMemoryAllocator<Key> >;
template<typename Key, typename Tp, typename Compare = std::less<Key> >
using SharedMap = std::map<Key, Tp, Compare, SharedMemoryAllocator<std::pair<const Key, Tp> > >;

void proc_create(pid_t* pid, std::function<void*(void*)> worker, void* args) {
    *pid = fork();
    if (*pid < 0) {
        return;
    }
    if (*pid == 0) {
        worker(args);
        exit(0);
    }
    return;
}

sem_t* proc_sem_init(int cnt = 1) {
    sem_t* sem = createSharedMemory(sem_t);
    sem_init(sem, 1, cnt);
    return sem;
}

void proc_sem_lock(sem_t* sem) {
    sem_wait(sem);
}

void proc_sem_unlock(sem_t* sem) {
    sem_post(sem);
}

int proc_getfd_from_pid(pid_t pid, int fd) {
    int pidfd = syscall(SYS_pidfd_open, pid, 0);
    if (pidfd == -1) return -1;
    return syscall(SYS_pidfd_getfd, fd, 0);
}

void proc_sendfd(int sockfd, int fd) {
    struct msghdr hdr;
    bzero(&hdr, sizeof(hdr));
    char buf[] = "x";
    struct iovec iov[1];
    iov->iov_base = buf;
    iov->iov_len = 1;
    hdr.msg_iov = iov;
    hdr.msg_iovlen = 1;
    struct cmsghdr *pmsg = (struct cmsghdr*)malloc(CMSG_LEN(sizeof(int)));
    pmsg->cmsg_len = CMSG_LEN(sizeof(int));
    pmsg->cmsg_level = SOL_SOCKET;
    pmsg->cmsg_type = SCM_RIGHTS;
    *(int*)CMSG_DATA(pmsg) = fd;
    hdr.msg_control = pmsg;
    hdr.msg_controllen = CMSG_LEN(sizeof(int));
    int ret = sendmsg(sockfd, &hdr, 0);
}

int proc_getfd(int sockfd) {
    struct msghdr hdr;
    bzero(&hdr, sizeof(hdr));
    char buf[1024] = {0};
    struct iovec iov[1];
    iov->iov_base = buf;
    iov->iov_len = 1024;
    hdr.msg_iov = iov;
    hdr.msg_iovlen = 1;
    struct cmsghdr *pmsg = (struct cmsghdr*)malloc(CMSG_LEN(sizeof(int)));
    pmsg->cmsg_len = CMSG_LEN(sizeof(int));
    pmsg->cmsg_level = SOL_SOCKET;
    pmsg->cmsg_type = SCM_RIGHTS; //传文件对象
    hdr.msg_control = pmsg;
    hdr.msg_controllen = CMSG_LEN(sizeof(int));
    int ret = recvmsg(sockfd, &hdr, 0);
    // printf("buf == %s, fdtorecv == %d\n", buf, *(int*)CMSG_DATA(pmsg));
    return *(int*)CMSG_DATA(pmsg);
}

extern char** environ;
char** cachedargv;
char* cachedlast = NULL;
bool proc_initedtitle = false;

void proc_inittitle(char** argv) {
    char* tmp = NULL;
    int siz = 0;

    for (int i = 0; environ[i] != NULL; i++) siz += strlen(environ[i]) + 1;

    tmp = new char[siz];
    if (tmp == NULL) return;

    cachedlast = argv[0];
    cachedargv = argv;
    for (int i = 0; argv[i] != NULL; i++) cachedlast += strlen(argv[i]) + 1;

    for (int i = 0; environ[i] != NULL; i++) {
        siz = strlen(environ[i]) + 1;
        cachedlast += siz;
        strncpy(tmp, environ[i], siz);
        environ[i] = tmp;
        tmp += siz;
    }

    cachedlast--;
    return;
}

void proc_settitle(const char* title) {
    char* tmp = cachedargv[0];
    strncpy(tmp, title, cachedlast - tmp);
    return;
}

const std::string singal_strings[] = {
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

std::string __builtin_proc_system2(std::string cmd) {
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

std::vector<std::string> __builtin_proc_explode(std::string seperator, std::string source) {
	std::string src = source; std::vector<std::string> res;
	while (src.find(seperator) != std::string::npos) {
		int wh = src.find(seperator);
		res.push_back(src.substr(0, src.find(seperator)));
		src = src.substr(wh + std::string(seperator).size());
	} res.push_back(src);
	return res;
}

std::vector<int> __builtin_proc_getpidsByPpid(int ppid) {
    std::string pidString = __builtin_proc_system2("ps -o pid= --ppid \"" + std::to_string(ppid) + "\"");
    auto pidStrings = __builtin_proc_explode("\n", pidString);
    std::vector<int> pids = { ppid };
    for (int i = 0; i < pidStrings.size(); i++) {
        int pid = atoi(pidStrings[i].c_str());
        if (pid == 0) continue;
        auto res = __builtin_proc_getpidsByPpid(pid);
        for (int j = 0; j < res.size(); j++) pids.push_back(res[j]);
    }
    return pids;
}

std::string __builtin_proc_readFile(std::string path) {
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

std::string __builtin_proc_sha256(std::string src) {
    unsigned char* sSHA = new unsigned char[32];
    SHA256((const unsigned char*)src.c_str(), src.length(), sSHA);
    std::string res;
    for (int i = 0; i < 32; i++) res += sSHA[i];
    delete[] sSHA;
    return res;
}

void proc_daemon(char** argv, const char* daemon_title, std::vector<std::string> hotreloads = {}) {
    int argc = 0;
    for (; argv[argc]; argc++) ;
	char *oldargv[argc + 1];
    for (int i = 0; i < argc; i++) {
        oldargv[i] = new char[strlen(argv[i])];
        memcpy(oldargv[i], argv[i], strlen(argv[i]));
    }
    oldargv[argc] = NULL;
    proc_inittitle(argv);
    pid_t pid = fork();
    std::vector<std::string> hashes;
    for (int i = 0; i < hotreloads.size(); i++) {
        std::string content = __builtin_proc_readFile(hotreloads[i]);
        std::string hash = __builtin_proc_sha256(content);
        hashes.push_back(hash);
    }
    if (pid > 0) {
        proc_settitle(daemon_title);
        auto pids = __builtin_proc_getpidsByPpid(pid);
        usleep(100 * 1000);
        while (true) {
            // 监视文件修改 => 热重载
            bool shouldHotReload = false;
            for (int i = 0; i < hotreloads.size(); i++) {
                std::string content = __builtin_proc_readFile(hotreloads[i]);
                std::string hash = __builtin_proc_sha256(content);
                if (hash != hashes[i]) {
                    std::cout << "\033[0;33m[Daemon] File \"" << hotreloads[i] << "\" updated, hot reloading...\033[0m" << std::endl;
                    shouldHotReload = true;
                    break;
                }
            }
            if (shouldHotReload) break;

            // 监视子进程状态
            int status;
            auto newpids = __builtin_proc_getpidsByPpid(pid);
            pid_t newpid = waitpid(pid, &status, WNOHANG);
            if (newpid == 0) {
                pids = newpids;
                usleep(100 * 1000);
                continue;
            }
            std::cout << "\033[0;31m[Daemon] Process PID = " << newpid << " exited";
            if ((status & 0x7f) == 0) {
                std::cout << ", exit code = " << ((status >> 8) & 0xff);
            } else {
                std::cout << " abnormally, exit signal = " << singal_strings[status & 0x3f];
            }
            std::cout << ", restarting...\033[0m" << std::endl;
            break;
        }
        for (int i = 0; i < pids.size(); i++) {
            int ret = kill(pids[i], SIGTERM);
            if (ret) {
                if (errno == ESRCH) continue;
                else assert(false);
            }
        }
        execv(oldargv[0], oldargv);
        exit(0);
    }
}