#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <semaphore.h>
#include <set>
#include <string.h>
#include <sys/socket.h>
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