#include <iostream>
#include "HttpServer.h"
#include "HttpContext.h"
#include "base/Logging.h"

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
    LOG_INFO << "HttpServer[" << m_server.getName() << "] starts.";
    m_server.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    // do nothing
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    HttpContext context(buf);
    context.handleHttpRequest();
    std::string http_response;
    context.getHttpResponseMessage(http_response);
    conn->send(http_response);
}