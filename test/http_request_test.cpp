#include <iostream>
#include <string>
#include "../HttpRequest.h"

int main() {
    // request:
    // "GET/HEAD www.baidu.com http/1.0\r\n"
    // "Connection: Keep-Alive\r\n"
    // "\r\n"
    std::string request = "HEAD www.baidu.com http/1.0\r\nConnection: Keep-Alive\r\n\r\n";
    HttpRequest req(0);
    Buffer& buffer = req.getBuffer();
    buffer.setContent(request);
    req.parseHTTPRequeset();

    METHOD method = req.getMethod();
    if (method != HEAD) { std::cout << "Unexpected method!" << std::endl; }
    HTTP_VERSION version = req.getVersion();
    if (version != HTTP_1_0) { std::cout << "Unexpected version!" << std::endl; }
    CONNECTION conn = req.getConnection();
    if (conn != KEEP_ALIVE) { std::cout << "Unexpected connection!" << std::endl; }
    std::string uri = req.getURI();
    if (uri != "www.baidu.com") { std::cout << "Unexpected URI!" << std::endl; }

    return 0;
}