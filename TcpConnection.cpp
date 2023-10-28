#include <unistd.h>
#include <sys/socket.h>
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "base/Logging.h"
#include "TimerHeap.h"


TcpConnection::TcpConnection(EventLoop* loop, std::string name, int sockfd) 
:   m_loop(loop),
    m_name(name),
    m_state(kConnecting),
    m_socket_ptr(new Socket(sockfd)),
    m_channel(new Channel(m_loop, sockfd)),
    m_input_buffer(),
    m_output_buffer(),
    m_context(new HttpContext(&m_input_buffer, &m_output_buffer)),
    m_alive(false),
    m_timeout(false)
{
    m_channel->setReadHandler(std::bind(&TcpConnection::handleRead, this));
    m_channel->setWriteHandler(std::bind(&TcpConnection::handleWrite, this));
    m_channel->setErrorHandler(std::bind(&TcpConnection::handleError, this));
    m_channel->setCloseHandler(std::bind(&TcpConnection::handleClose, this));
    LOG_INFO << "TcpConnection::ctor[" << m_name << "] at" << this << " fd = " << sockfd;
    // setTcpKeepAlive(true);
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
        handleHttpRequest();
        // if get a keep-alive request
        if (m_alive == false && m_context->isKeepAlive()) {
            m_alive = true;
            m_loop->runAfter(TimerHeap::defaultTimeDelay,
                        std::bind(&TcpConnection::closeLongConnection, shared_from_this()));
        }
        // if the connection is long and not timeout
        // if (m_alive && m_timeout == false) {
        //     m_context->reset();
        //     m_channel->enableReading();
        // }
        handleWrite();
    }
    else if (bytes == 0) {
        // the peer closed the connection
        handleClose();
    }
    else if (bytes < 0) {
        LOG_ERROR << "bytes get -1";
        handleError();
    }
}

void TcpConnection::handleWrite() {
    // write back reponse data to the client    
    int sockfd = m_socket_ptr->getFd();
    ssize_t bytes = m_output_buffer.writeToFd(sockfd);
    if (bytes < 0) {
        LOG_ERROR << "write response back to the peer client failed";
        handleError();
    }
    else if (bytes == 0 && m_output_buffer.readableBytes() == 0) {
        // the peer client closed the connection 
        // and the response has been sent
        handleClose();
    }
    else if (bytes == 0 && m_output_buffer.readableBytes() > 0) {
        // the peer client close the connection
        // but the respones has not been sent completely
        LOG_ERROR << "the peer client closed the connection";
        handleError();
    }
    else if (bytes > 0 && m_output_buffer.readableBytes() > 0) {
        // if any byte of the response is left, update the socket fd with EPOLLOUT event in its epoll
        m_channel->enableWriting(); 
    }
    else if (bytes > 0 && m_alive && m_timeout == false) {
        // long connection and not timeout
        m_context->reset();
        m_channel->enableReading();
    }
    else {
        // other situations
        handleClose();
    }
}

void TcpConnection::handleClose() {
    LOG_DEBUG << "TcpConnection::handleClose()";
    // LOG_DEBUG << "assertInLoopThread() begin";
    m_loop->assertInLoopThread();
    
    // if the connection is a long connection, close it after timeout
    if (m_alive && m_timeout == false) return;
    // if (m_timeout == false && m_alive == false && m_context->isKeepAlive()) {
    //     m_alive = true;
    //     // set a timer for the connection
    //     m_loop->runAfter(TimerHeap::defaultTimeDelay, 
    //                     std::bind(&TcpConnection::closeLongConnection, shared_from_this()));
    //     // ready for new requests
    //     m_context->reset();
    //     m_channel->enableReading();
    //     return;
    // }
    // // if the connection is a long connection, close it when it is timeout
    // if (m_alive && m_timeout == false) {
    //     m_context->reset();
    //     m_channel->enableReading();
    //     return;
    // }
    LOG_DEBUG << "TcpConnection::handleClose() is called by loop " << m_loop;
    if (m_state != kConnected && m_state != kDisconnecting) {
        // LOG_FATAL << "TcpConnection::handleClose(): state should be kConnected instead of " << stateToString();
        LOG_FATAL << "TcpConnection [" << m_name << "]: should be kConnected or kDisconnecting instead of " << stateToString();
    }
    setState(kDisconnected);
    // unregister fd from epoll
    m_channel->disableAll();
    // close a long connection which is timeout immediately
    if (m_alive && m_timeout) {
        m_socket_ptr->shutdown();
    }
    // erase this connection from server's connection map
    TcpConnectionPtr guard_this(shared_from_this());
    // call TcpServer::removeConnection()
    m_close_callback(guard_this);
}

void TcpConnection::closeLongConnection() {
    m_timeout = true;
    handleClose();
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
    // close the error connection
    handleClose();
}

void TcpConnection::handleHttpRequest() {
    m_context->work();
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