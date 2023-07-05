#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H
#include <memory>
#include <functional>
#include <string>

#include "Buffer.h"
#include "base/noncopyable.h"

class Socket;
class EventLoop;
class Channel;
class TcpConnection;


typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&, Buffer*)> MessageCallback;


/*class TcpConnection: encapsulation for a tcp connection and it's unreused*/
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
    public:
        TcpConnection(EventLoop* loop, std::string name, int sockfd);
        ~TcpConnection();


        // internal use only for class TcpServer
        void sestCloseCallback(const CloseCallback& cb) { m_close_callback = cb; }

        void setConnectionCallback(const ConnectionCallback& cb) { m_conn_callback = cb; }
        void setMessageCallback(const MessageCallback& cb) { m_message_callback = cb; }
        
        // called when TcpServer accepts a new connection and called only once for one connection
        void connectEstablished();
        // called when TcpServer remove one connnection and called only once for one connnection
        void connectDestroyed();

        const std::string& getName() const { return m_name; }

        void send(const std::string& message);
        void shutdown();

        void setTcpNoDelay(bool on);
        void setTcpKeepAlive(bool on);

        EventLoop* getLoop() const { return m_loop; }


    private:
        enum StateE { kConnecting, kConnected, kDisconnected, kDisconnecting };
        void setState(StateE state) { m_state = state; }
        void handleRead();
        void handleWrite();
        void handleClose();
        void handleError();
        void sendInLoop(const std::string& message);
        void shutdownInLoop();
        


        // accptor loop
        EventLoop* m_loop;
        std::string m_name;
        StateE m_state;
        // client
        std::unique_ptr<Socket> m_socket_ptr;
        // acceptor channel
        std::unique_ptr<Channel> m_channel;
        Buffer m_input_buffer;
        Buffer m_output_buffer;
        // ???build a new connnection
        ConnectionCallback m_conn_callback;
        // ???deal with info from client, the call back is set by the user
        MessageCallback m_message_callback;
        // close callback is bound to TcpServer::removeConnection()
        CloseCallback m_close_callback;
}; // class TcpConnection
#endif // TCP_CONNECTION_H