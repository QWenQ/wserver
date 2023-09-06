#include <unistd.h>
#include <sys/socket.h>
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "base/Logging.h"

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
    LOG_INFO << "TcpConnection::ctor[" << m_name << "] at" << this << " fd = " << sockfd;
    // create a long tcp connection
    setTcpKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_INFO << "TcpConnection::dtor[" << m_name << "] at " << this
                << " fd = " << m_channel->fd()
                << " state = " << stateToString();
    if (m_state != kDisconnected) {
        LOG_FATAL << "TcpConnection::~TcpConnection() error!";
    }
}

std::string TcpConnection::stateToString() const {
    if (m_state == kConnecting) {
        return "kConnecting";
    }
    else if (m_state == kConnected) {
        return "kConnected";
    }
    else if (m_state == kDisconnected) {
        return "kDisconnected";
    }
    else if (m_state == kDisconnecting) {
        return "kDisconnecting";
    }
    else {
        return "Uknown state";
    }
}



void TcpConnection::connectEstablished() {
    setState(kConnected);
    m_channel->enableReading();
}

void TcpConnection::connectDestroyed() {
    LOG_DEBUG << "TcpConnection::connectDestroyed()";
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_loop->assertInLoopThread();
    if (m_state == kConnected) {
        setState(kDisconnected);
        m_channel->disableAll();
    }
    m_channel->remove();
}

void TcpConnection::handleRead() {
    LOG_DEBUG << "TcpConnection::handleRead()";
    int sockfd = m_socket_ptr->getFd();
    ssize_t bytes = m_input_buffer.readFromFd(sockfd);
    LOG_DEBUG << "read bytes " << bytes;
    if (bytes > 0) {
        m_message_callback(shared_from_this(), &m_input_buffer);
    }
    else if (bytes == 0) {
        handleClose();
    }
    else {
        LOG_ERROR << "TcpConnection::handleRead";
        handleError();
    }
}

void TcpConnection::handleWrite() {
    // LOG_DEBUG << "assertInLoopThread() begin";
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
    else {
        LOG_ERROR << "TcpConnection::handleWrite";
    }
}

void TcpConnection::handleClose() {
    LOG_DEBUG << "TcpConnection::handleClose()";
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_loop->assertInLoopThread();
    LOG_DEBUG << "TcpConnection::handleClose() is called by loop " << m_loop;
    if (m_state != kConnected && m_state != kDisconnecting) {
        LOG_FATAL << "TcpConnection::handleClose(): state should be kConnected instead of " << stateToString();
    }
    setState(kDisconnected);
    // unregister fd from epoll
    m_channel->disableAll();
    // erase this connection from server's connection map
    TcpConnectionPtr guard_this(shared_from_this());
    // call TcpServer::removeConnection()
    m_close_callback(guard_this);
}

int getSocketError(int fd) {
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
    int ret = ::getsockopt(fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
    if (ret < 0) {
        return errno;
    }
    return optval;
}


void TcpConnection::handleError() {
    int err = getSocketError(m_channel->fd());
    char buf[512];
    const char* err_str = strerror_r(err, buf, sizeof(buf));

    LOG_ERROR << "TcpConnection::handleError [" << m_name 
                << "] - SO_ERROR = " << err << " " << err_str;
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
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_loop->assertInLoopThread();
    if (m_state == kDisconnected) {
        LOG_ERROR << "disconnected, give up writing";
        return;
    }
    ssize_t bytes = 0;
    if (!m_channel->isWriting() || m_output_buffer.readableBytes() != 0) {
        bytes = ::write(m_socket_ptr->getFd(), message.data(), message.size());
        if (bytes < 0) {
            if (errno != EWOULDBLOCK) {
                LOG_ERROR << "TcpConnection::sendInLoop";
            }
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
    // LOG_DEBUG << "assertInLoopThread() begin";
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