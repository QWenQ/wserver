#ifndef SOCKET_H
#define SOCKET_H

#include <fcntl.h>
#include <errno.h>

class Socket {
    public:
        Socket();
        Socket(int fd, bool closed = false);
        ~Socket();

        void bind();
        void listen();
        int accept();
        void close();

        int getFd() const { return m_sockfd; }
        void shutdownWrite();
        void shutdown();
        // no Nagle algorithm
        void setTcpNoDelay(bool on);
        // enable sending of keep-alive messages on connection-oriented sockets periodically.
        void setTcpKeepAlive(bool on);
        // set non block IO
        void setNonBlock(bool on);
        void setReuseAddr(bool on);
        void setReusePort(bool on);
        void setLinger(bool on);

        bool isValid() const { return ::fcntl(m_sockfd, F_GETFL) != -1 || errno != EBADF; }


    private:
        int m_sockfd;
        bool m_closed;
}; // class Socket
#endif // SOCKET_H