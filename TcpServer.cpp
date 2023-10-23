#include <functional>
#include "Acceptor.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"
#include "base/Logging.h"

void defaultConnectionCallback(const TcpConnectionPtr& conn) {
    LOG_INFO << "Server: " << conn->getName() << " build a new connection./n";
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf) {
    buf->retrieveAll();
}

TcpServer::TcpServer(EventLoop* loop, const std::string& name, bool reuse_port) 
:   m_main_loop(loop),
    m_name(name),
    m_acceptor_ptr(new Acceptor(m_main_loop, reuse_port)),
    m_pool_ptr(new EventLoopThreadPool(m_main_loop)),
    m_conn_callback(defaultConnectionCallback),
    m_message_callback(defaultMessageCallback),
    m_started(false),
    m_next_conn_id(1),
    m_connections()
{
    m_acceptor_ptr->setNewConnnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_main_loop->assertInLoopThread();
    for (auto& it : m_connections) {
        TcpConnectionPtr conn = it.second;
        it.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
    }
}

void TcpServer::newConnection(int fd) {
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_main_loop->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof(buf), "#conn-id: %d", m_next_conn_id);
    ++m_next_conn_id;
    std::string conn_name = m_name + buf;
    EventLoop* io_loop = m_pool_ptr->getNextLoop();
    LOG_INFO << "server: " << m_name << "- new connection " << conn_name;

    TcpConnectionPtr conn(new TcpConnection(io_loop, conn_name, fd));
    m_connections[conn_name] = conn;
    conn->setConnectionCallback(m_conn_callback);
    conn->setMessageCallback(m_message_callback);
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    io_loop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn.get()));
}

void TcpServer::removeConnection(const TcpConnectionPtr& connection) {
    LOG_DEBUG << "TcpServer::removeConnection()";
    m_main_loop->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, connection));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& connection) {
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_main_loop->assertInLoopThread();
    LOG_INFO << "server: " << m_name << " - remove connection " << connection->getName();
    m_connections.erase(connection->getName());
    EventLoop* io_loop = connection->getLoop();
    io_loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, connection));
}

void TcpServer::start() {
    m_started = true;
    // start working threads
    m_pool_ptr->start();
    // get new client sockets from the web
    m_main_loop->runInLoop(std::bind(&Acceptor::listen, m_acceptor_ptr.get())); 
}


void TcpServer::setThreadNums(int nums) {
    m_pool_ptr->setThreadNums(nums);
}