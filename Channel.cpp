#include <sys/epoll.h>
#include "Channel.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
:   m_loop(loop),
    m_fd(fd),
    m_events(0),
    m_revents(0),
    m_index(-1)
{
    // todo
}

Channel::~Channel() {
    // todo
}

void Channel::update() {
    m_loop->updateChannel(this); 
}

void Channel::handleEvent() {
    if (m_revents & EPOLLERR) {
        handleErrorEvent();
    }
    if (m_revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        handleReadEvent();
    }
    else if (m_revents & EPOLLOUT) {
        handleWriteEvent();
    }
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