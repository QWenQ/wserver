#include <iostream>
#include <unistd.h>
#include <string>
#include <chrono>
#include <sys/resource.h>
#include "../AsyncLog.h"
#include "../Logging.h"

/*
const off_t kRollSize = 500 * 1000 * 1000;
AsyncLog* g_async_log = NULL;
void asyncOutput(const char* msg, size_t len) {
    g_async_log->append(msg, len);
}

void asyncFlush() {
    g_async_log->flush();
}

void bench(bool long_log) {
    int cnt = 0;
    const int kBatch = 1000;
    std::string empty = " ";
    std::string long_str(3000, 'X');
    long_str += " ";
    for (int i = 0; i < 30; ++i) {
        const auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        for (int j = 0; j < kBatch; ++j) {
            LOG_INFO << "hello, world" << (long_log ? long_str : empty) << cnt;
            ++cnt;
        }
        const auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        std::cout << "lapsed time: " << (end - start) << "ms\n";
        sleep(1);
    }
}
*/


int main(int argc, char* argv[]) {
    /*
    // Logger::setOutputFunc(asyncOutput);
    {
        size_t kOneGB = 1000 * 1024 * 1024;
        rlimit rl = {2 * kOneGB, 2 * kOneGB};
        setrlimit(RLIMIT_AS, &rl);
    }
    printf("pid: %d\n", getpid());
    char filename[256] = {'\0'};
    // strncpy(filename, argv[0], sizeof(argv[0]));
    strncpy(filename, argv[0], strlen(argv[0]));
    AsyncLog asylog(::basename(filename), kRollSize);
    g_async_log = &asylog;
    asylog.start();
    bool long_log = (argc > 1);
    bench(long_log);
    */
    LOG_INFO << "hello world!";
    LOG_FATAL << "log fatal test";
    return 0;
}



