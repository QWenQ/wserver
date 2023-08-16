#include <iostream>
#include "HttpServer.h"
#include "HttpContext.h"

HttpServer::HttpServer(EventLoop* loop)
:   m_server(loop, "Http Server", true)
{ 
    m_server.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer() = default;

void HttpServer::start() {
    m_server.start();
}


void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    // todo: log the connnection 
    std::cout << "A http connnection has been constructed!" << std::endl;
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    HttpContext context(buf);
    context.handleHttpRequest();
    std::string http_response;
    context.getHttpResponseMessage(http_response);
    conn->send(http_response);
}