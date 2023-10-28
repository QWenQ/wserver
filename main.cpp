#include "TcpServer.h"
#include "EventLoop.h"

int main(int argc, char* argv[]) {
    Logger::setLogLevel(Logger::ERROR);
    EventLoop loop;
    TcpServer server(&loop, "myserver", true);
    // default thread nums == 4
    // server.setThreadNums(2);
    // start threadpool and listen for new clients
    server.start();
    // main loop starts working to allocate new clients to threadpool
    loop.loop();
    return 0;
}