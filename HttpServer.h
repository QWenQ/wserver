#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "TcpServer.h"
#include "TcpConnection.h"
#include "base/noncopyable.h"

class HttpContext;

class HttpServer : noncopyable {
    public:
        typedef std::function<void(HttpContext* context)> HttpCallback;

        HttpServer(EventLoop* loop);
        ~HttpServer();

        void start();
    private:
        // report build a new tcp connection
        void onConnection(const TcpConnectionPtr&);
        // handle the data saved in the buffer and write a response back to client
        void onMessage(const TcpConnectionPtr&, Buffer*);

        TcpServer m_server;

}; // class HttpServer
#endif // HTTPSERVER_H