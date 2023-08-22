#include "Acceptor.h"
#include "base/Logging.h"

Acceptor::Acceptor(EventLoop* loop, bool reuse_port) 
:   m_loop(loop),
    m_socket(),
    m_accept_channel(m_loop, m_socket.getFd()),
    m_new_conn_callback(),
    m_listenning(false)
{
    m_socket.setReuseAddr(true);
    m_socket.setReusePort(reuse_port);
    m_accept_channel.setReadHandler(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    m_accept_channel.disableAll();
    m_accept_channel.remove();
}


void Acceptor::listen() {
    m_loop->assertInLoopThread();
    // bind the address to listen socket
    m_socket.bind();
    m_listenning = true;
    m_socket.listen();
    m_accept_channel.enableReading();
}

void Acceptor::handleRead() {
    m_loop->assertInLoopThread();
    int connfd = m_socket.accept();
    if (connfd >= 0) {
        if (m_new_conn_callback) {
            m_new_conn_callback(connfd);
        }
        else {
            ::close(connfd);
        }
    }
    else {
        LOG_ERROR << "in Acceptor::handleRead";
    }
}