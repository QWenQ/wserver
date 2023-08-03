#include <iostream>
#include "HttpServer.h"
#include "HttpRequest.h"

HttpServer::HttpServer(EventLoop* loop)
:   m_loop(loop),
    m_server(loop, "Http Server", true)
{ 
    m_server.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer() {
    // todo
}

void HttpServer::start() {
    m_server.start();
}


void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    // todo
    std::cout << "A http connnection has been constructed!" << std::endl;
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    HttpRequest request(buf);
    request.start();
    std::string http_response = request.getResponseMessage();
    conn->send(http_response);
}