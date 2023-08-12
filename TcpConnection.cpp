#include <unistd.h>
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

TcpConnection::TcpConnection(EventLoop* loop, std::string name, int sockfd) 
:   m_loop(loop),
    m_name(name),
    m_state(kConnecting),
    m_socket_ptr(new Socket(sockfd)),
    m_channel(new Channel(m_loop, sockfd)),
    m_input_buffer(),
    m_output_buffer()
{
    m_channel->setReadHandler(std::bind(&TcpConnection::handleRead, this));
    m_channel->setWriteHandler(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setErrorHandler(std::bind(&TcpConnection::handleError, this));
    m_channel->setCloseHandler(std::bind(&TcpConnection::handleClose, this));
    // create a long tcp connection
    setTcpKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    assert(m_state == kDisconnected);
}

void TcpConnection::connectEstablished() {
    // todo
    setState(kConnected);
    m_channel->enableReading();
}

void TcpConnection::connectDestroyed() {
    m_loop->assertInLoopThread();
    assert(m_state == kConnected);
    setState(kDisconnected);
    m_channel->disableAll();
    m_conn_callback(shared_from_this());
    m_channel->remove();
}

void TcpConnection::handleRead() {
    int sockfd = m_socket_ptr->getFd();
    ssize_t bytes = m_input_buffer.readFromFd(sockfd);
    if (bytes > 0) {
        m_message_callback(shared_from_this(), &m_input_buffer);
    }
    else if (bytes == 0) {
        handleClose();
    }
    else {
        handleError();
    }
}

void TcpConnection::handleWrite() {
    m_loop->assertInLoopThread();
    ssize_t bytes = m_output_buffer.writeToFd(m_socket_ptr->getFd());
    if (bytes > 0) {
        if (m_output_buffer.readableBytes() == 0) {
            m_channel->disableWriting();
            if (m_state == kDisconnecting) {
                shutdownInLoop();
            }
        }
    }
}

void TcpConnection::handleClose() {
    m_loop->assertInLoopThread();
    assert(m_state == kConnected);
    // the dtor of class Socket will close socket fd
    m_channel->disableAll();
    m_close_callback(shared_from_this());
}

void TcpConnection::handleError() {
    // todo: nothing but log the error
}

void TcpConnection::send(const std::string& message) {
    if (m_state == kConnected) {
        if (m_loop->isInLoopThread()) {
            sendInLoop(std::move(message)); 
        }
        else {
            m_loop->runInLoop(std::bind(&TcpConnection::sendInLoop, shared_from_this(), std::move(message)));
        }
    }
}

void TcpConnection::sendInLoop(const std::string& message) {
    // todo
    m_loop->assertInLoopThread();
    ssize_t bytes = 0;
    if (!m_channel->isWriting() || m_output_buffer.readableBytes() != 0) {
        bytes = ::write(m_socket_ptr->getFd(), message.data(), message.size());
        if (bytes < 0) {
            perror("write() error!");
        }
    }

    // if not all data has been transferred by the write(), save it to the output buffer
    if (static_cast<std::string::size_type>(bytes) < message.size()) {
        m_output_buffer.append(message.substr(bytes));
        // register EPOLLOUT event to send the rest data
        if (!m_channel->isWriting()) {
            m_channel->enableWriting();
        }
    }
}

void TcpConnection::shutdown() {
    if (m_state == kConnected) {
        setState(kDisconnecting);
        m_loop->runInLoop(std::bind(&TcpConnection::shutdownInLoop, shared_from_this()));
    }
}

void TcpConnection::shutdownInLoop() {
    m_loop->assertInLoopThread();
    if (!m_channel->isWriting()) {
        m_socket_ptr->shutdownWrite();
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    m_socket_ptr->setTcpNoDelay(on);
}

void TcpConnection::setTcpKeepAlive(bool on) {
    m_socket_ptr->setTcpKeepAlive(on);
}