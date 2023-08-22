#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <memory>
#include <functional>
#include <vector>
#include <string>
#include <map>

#include "base/noncopyable.h"

class EventLoop;
class EventLoopThreadPool;
class TcpConnection;
class Buffer;
class Acceptor;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*)> MessageCallback;

class TcpServer : noncopyable {
    public:
        TcpServer(EventLoop* loop, const std::string& name, bool reuse_port);
        ~TcpServer();

        // start the server
        void start();

        void setConnectionCallback(const ConnectionCallback& cb) { m_conn_callback = cb; }
        void setMessageCallback(const MessageCallback& cb) { m_message_callback = cb; }


        void setThreadNums(int nums);
        const std::string& getName() const { return m_name; }

        

    private:
        void newConnection(int fd);
        void removeConnection(const TcpConnectionPtr& connection);
        void removeConnectionInLoop(const TcpConnectionPtr& connection);

        typedef std::map<std::string, TcpConnectionPtr> ConnectionMap; 

        // the loop is the owner of acceptor
        EventLoop* m_main_loop;
        std::string m_name;
        std::unique_ptr<Acceptor> m_acceptor_ptr;
        std::unique_ptr<EventLoopThreadPool> m_pool_ptr;
        // get info of client, set by the user
        ConnectionCallback m_conn_callback;
        // parse the info of client, set by the user
        MessageCallback m_message_callback;
        bool m_started;
        int m_next_conn_id;
        ConnectionMap m_connections;
}; // class TcpServer
#endif // TCP_SERVER_H