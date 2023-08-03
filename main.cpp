#include "HttpServer.h"
#include "EventLoop.h"

int main(int argc, char* argv[]) {
    EventLoop loop;
    HttpServer server(&loop);
    server.start();
    loop.loop();
    return 0;
}