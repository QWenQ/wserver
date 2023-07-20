#include "HttpServer.h"

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


void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    // todo
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buf) {
    // todo
    // parse http request 
    // produce a http response
    // write back http response to the client
    std::string http_response = "todo";
    conn->send(http_response);
}