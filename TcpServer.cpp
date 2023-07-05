#include <functional>
#include "Acceptor.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"

TcpServer::TcpServer(EventLoop* loop, const std::string& name, bool reuse_port) 
:   m_acceptor_loop(loop),
    m_name(name),
    m_acceptor_ptr(new Acceptor(m_acceptor_loop, reuse_port)),
    m_pool_ptr(new EventLoopThreadPool()),
    m_started(false),
    m_next_conn_id(1),
    m_connections()
{
    m_acceptor_ptr->setNewConnnectionCallback(&std::bind(TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
    m_acceptor_loop->assertInLoopThread();
    for (auto& it : m_connections) {
        TcpConnectionPtr conn = it.second;
        it.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::newConnection(int fd) {
    m_acceptor_loop->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", m_next_conn_id);
    ++m_next_conn_id;
    std::string conn_name = m_name + buf;
    EventLoop* io_loop = m_pool_ptr->getNextLoop();
    TcpConnectionPtr conn(new TcpConnection(io_loop, conn_name, fd));
    m_connections[conn_name] = conn;
    conn->setConnectionCallback(m_conn_callback);
    conn->setMessageCallback(m_message_callback);
    conn->sestCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    io_loop->runInLoop(std::bind(TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& connection) {
    m_acceptor_loop->runInLoop(std::bind(TcpServer::removeConnectionInLoop, this, connection));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& connection) {
    m_acceptor_loop->assertInLoopThread();
    m_connections.erase(connection->getName());
    EventLoop* io_loop = connection->getLoop();
    io_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, connection));
}

void TcpServer::start() {
    m_started = true;
    m_pool_ptr->start();
    m_acceptor_loop->runInLoop(std::bind(&Acceptor::listen, m_acceptor_ptr)); 
}
