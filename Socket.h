#ifndef SOCKET_H
#define SOCKET_H

class Socket {
    public:
        Socket();
        Socket(int fd);
        ~Socket();

        void bind();
        void listen();
        int accept();
        void close();

        int getFd() const { return m_sockfd; }
        void shutdownWrite();
        // no Nagle algorithm
        void setTcpNoDelay(bool on);
        // enable sending of keep-alive messages on connection-oriented sockets periodically.
        void setTcpKeepAlive(bool on);
        // set non block IO
        void setNonBlock(bool on);
        void setReuseAddr(bool on);
        void setReusePort(bool on);
        void setLinger(bool on);


    private:
        int m_sockfd;
}; // class Socket
#endif // SOCKET_H