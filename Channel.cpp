#include <sys/epoll.h>
#include "Channel.h"
#include "base/Logging.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
:   m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1)
{ }

Channel::~Channel() {
    if (m_event_handling == true) {
        LOG_ERROR << "Channel::~Channel() error!";
    }
    remove();
}

void Channel::update() {
    m_loop->updateChannel(this); 
}

void Channel::handleEvent() {
    m_event_handling = true;
    // when the peer closes the connection and no data in the read buffer
    if (m_revents & EPOLLHUP && !(m_revents & EPOLLIN)) {
        handleCloseEvent();
    }
    if (m_revents & EPOLLERR) {
        handleErrorEvent();
    }
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        handleReadEvent();
    }
    if (m_revents & EPOLLOUT) {
        handleWriteEvent();
    }
    m_event_handling = false;
}

void Channel::handleReadEvent() {
    if (m_read_handler) {
        m_read_handler();
    }
}

void Channel::handleWriteEvent() {
    if (m_write_handler) {
        m_write_handler();
    }
}

void Channel::handleConnectionEvent() {
    if (m_conn_handler) {
        m_conn_handler();
    }
}

void Channel::handleErrorEvent() {
    if (m_error_handler) {
        m_error_handler();
    }
}

void Channel::handleCloseEvent() {
    if (m_close_callback) {
        m_close_callback();
    }
}

void Channel::setReadHandler(const Callback& read_handler) {
    m_read_handler = read_handler;
}

void Channel::setWriteHandler(const Callback& write_handler) {
    m_write_handler = write_handler;
}

void Channel::setConnHandler(const Callback& conn_handler) {
    m_conn_handler = conn_handler;
}

void Channel::setErrorHandler(const Callback& error_handler) {
    m_error_handler = error_handler;
}

void Channel::setCloseHandler(const Callback& close_handler) {
    m_close_callback = close_handler;
}

void Channel::enableReading() {
    m_events |= kReadEvent;
    update();
}

void Channel::disableReading() {
    m_events &= ~kReadEvent;
    update();
}

void Channel::disableAll() {
    m_events = kNoneEvent;
    update();
}

void Channel::enableWriting() {
    m_events |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    m_events &= ~kNoneEvent;
    update();
}

bool Channel::isWriting() {
    return m_events & kWriteEvent;
}

void Channel::remove() {
    m_loop->assertInLoopThread();
    setIndex(kDeleted);
    m_loop->removeChannel(this);
}