#include "HttpServer.h"
#include "EventLoop.h"

int main(int argc, char* argv[]) {
    // debug
    Logger::setLogLevel(Logger::DEBUG);
    EventLoop loop;
    HttpServer server(&loop);
    // debug
    server.setThreadNums(2);
    server.start();
    loop.loop();
    return 0;
}