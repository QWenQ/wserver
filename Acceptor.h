#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include <sys/types.h>
#include <sys/socket.h>

#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"

class Acceptor {
    public:
        typedef std::function<void(int sockfd)> NewConnectionCallback;

        Acceptor(EventLoop* loop, bool reuse_port);
        ~Acceptor();

        void setNewConnnectionCallback(const NewConnectionCallback& cb) {
            m_new_conn_callback = cb;
        }

        bool listenning() const { return m_listenning; }
        void listen();
    private:
        // call accept() and new connection callback
        void handleRead();

        EventLoop* m_loop;
        Socket m_socket;
        Channel m_accept_channel;
        // new connection callback is bound to TcpConnection::m_conn_callback
        NewConnectionCallback m_new_conn_callback;
        bool m_listenning;
}; // class Acceptor
#endif // ACCEPTOR_H