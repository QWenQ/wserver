#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "TcpServer.h"
#include "TcpConnection.h"

class HttpServer {
    public:
        HttpServer(EventLoop* loop);
        ~HttpServer();

        void start();
    private:
        // report build a new tcp connection
        void onConnection(const TcpConnectionPtr&);
        // handle the data saved in the buffer and write a response back to client
        void onMessage(const TcpConnectionPtr&, Buffer*);

        EventLoop* m_loop;
        TcpServer m_server;
}; // class HttpServer
#endif // HTTPSERVER_H