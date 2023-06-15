#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>

#include "EventLoop.h"

class Channel {
    public:
        typedef std::function<void()> Callback;

        Channel(EventLoop* loop, int fd);
        ~Channel();

        void handleEvent();
        void handleReadEvent();
        void handleWriteEvent();
        void handleConnectionEvent();
        void handleErrorEvent();


        void setReadHandler(const Callback& read_handler);
        void setWriteHandler(const Callback& write_handler);
        void setConnHandler(const Callback& conn_handler);
        void setErrorHandler(const Callback& error_handler);


        void setRevents(uint32_t revents) { m_revents = revents; }
        int fd() const { return m_fd; }
        int index() const { return m_index; }
        int events() const { return static_cast<int>(m_events); }

        void setIndex(int index) { m_index = index; }

        bool isNoneEvent() const { return m_events == kNoneEvent; }

        EventLoop* ownerLoop() const { return m_loop; }

    private:
        void update();
        
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop* m_loop;
        const int m_fd;
        // the I/O events interested by the user
        uint32_t m_events;
        // the real I/O events happenning
        uint32_t m_revents;
        int m_index;

        Callback m_read_handler;
        Callback m_write_handler;
        Callback m_conn_handler;
        Callback m_error_handler;

}; // class Channel

#endif // CHANNEL_H