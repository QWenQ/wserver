#include <sys/types.h>
#include <sys/socket.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>

#include "Socket.h"

const char* ip = "127.0.0.1";
const unsigned short port = 1234;

// SOCK_NONBLOCK: nonblock socket
// SOCK_CLOEXEC: close the socket in child process when calling fork()
Socket::Socket() {    
    m_sockfd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (m_sockfd < 0) {
        perror("socket() error!");
    }
}

Socket::Socket(int fd) 
:   m_sockfd(fd)
{ }

Socket::~Socket() {
    ::close(m_sockfd); 
}

void Socket::bind() {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    inet_pton(PF_INET, ip, &addr.sin_addr);
    int ret = ::bind(m_sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind() error!");
    }
}

void Socket::listen() {
    int ret = ::listen(m_sockfd, 5);
    if (ret != 0) {
        // todo: log error
    }
}

int Socket::accept() {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len;
    int new_conn = ::accept(m_sockfd, (struct sockaddr*)&addr, &addr_len);
    if (new_conn < 0) {
        int saved_errno = errno;
        switch (saved_errno) {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: 
            case EPERM:
            case EMFILE: break; 
            default: perror("accept() error!");
        }
    }
    return new_conn;
}

void Socket::close() {
    ::close(m_sockfd);
}

void Socket::shutdownWrite() {
    // shutdown() will close the socket immediately in the spcified way 
    // instead of decreasing the reference count of the socket like close()
    if (::shutdown(m_sockfd, SHUT_WR) < 0) {
        perror("shutdown() error!");
    }
}

void Socket::setTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
    if (ret < 0) {
        perror("set TCP no delay error!");
    }
}

void Socket::setTcpKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    if (ret < 0) {
        perror("set TCP keep alive error!");
    }
}

void Socket::setNonBlock(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SOCK_NONBLOCK, &optval, sizeof(optval));
    if (ret < 0) {
        perror("set non block error!");
    }
}

void Socket::setReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if (ret < 0) {
        perror("set reuse address error!");
    }
}

void Socket::setReusePort(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    if (ret < 0) {
        perror("set reuse port error!");
    }
}

void Socket::setLinger(bool on) {
    // todo
}

void Socket::setKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
    if (ret < 0) {
        perror("set keep alive error!");
    }
}