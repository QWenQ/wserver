#include <iostream>
#include <chrono>
#include <iomanip>
#include "../Logging.h"
#include "../LogFile.h"

int g_total;
FILE* g_file;
std::unique_ptr<LogFile> g_log_file;

void dummyOutput(const char* msg, size_t len) {
    g_total += len;
    if (g_file) {
        ::fwrite(msg, 1, len, g_file);
    }
    else if (g_log_file) {
        g_log_file->append(msg, len);
    }
}


void bench(const char* type) {
    Logger::setOutputFunc(dummyOutput);
    g_total = 0;
    // record start time
    auto start = std::chrono::high_resolution_clock::now();
    // log 
    int items = 1000 * 1000;
    // int items = 1000;
    for (int i = 0; i < items; ++i) {
        LOG_INFO << "Hello, world" << " 0123456789" << i;
    }
    // record end time
    auto end = std::chrono::high_resolution_clock::now();
    // print 
    std::chrono::duration<double> lapsed = end - start;
    std::cout << "type: " << type 
                << std::fixed << std::left
                << std::setprecision(6)
                << "\nlapsed time: " << lapsed.count() << " seconds"
                << std::setprecision(2)
                << "\nmsg rate: " << items / lapsed.count() << "msg/s" 
                << "\nbyte rate: " << g_total / lapsed.count() / (1024 * 1024) << "MiB/s"
                << std::endl << std::endl;
}

int main() {

    // bench for nop
    bench("nop");
    // bench for log file
    char buffer[64 * 1024];
    // data write into /dev/null will be discarded
    g_file = fopen("/dev/null", "w");
    setbuffer(g_file, buffer, sizeof(buffer));
    bench("/dev/null");
    fclose(g_file);

    g_file = fopen("/tmp/log", "w");
    setbuffer(g_file, buffer, sizeof(buffer));
    bench("/tmp/log");
    fclose(g_file);

    // TODO:
    // bench for class LogFile
    g_file = NULL;
    // roll size 50
    g_log_file.reset(new LogFile("test_log_st", 50 * 1000 * 1000));
    bench("test_log_st");

    return 0;
}