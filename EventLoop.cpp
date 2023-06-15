#include <assert.h>
#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"

// the thread variable is an independent entity for each thread
// and the variables of thread do not interfere with each other.
__thread EventLoop* t_loopInThisThread = 0;

const int EventLoop::kPollTimeoutMs = 5000;

EventLoop::EventLoop(Epoll* epoll)
:   m_looping(false),
    m_quit(false),
    m_tid(gettid()),
    m_epoll(new Epoll(this)),
    m_active_channels()
{
    if (t_loopInThisThread) {
        // log: current thread has already owned an EventLoop ojbect
    }
    else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!m_looping);
    t_loopInThisThread = NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread() {
    // todo
}


void EventLoop::loop() {
    assert(!m_looping);
    assertInLoopThread();
    m_looping = true;
    m_quit = false;
    while (!m_quit) {
        m_active_channels.clear();
        m_epoll->poll(kPollTimeoutMs, &m_active_channels);
        // handle I/O event
        for (Channel* it : m_active_channels) {
            it->handleEvent();
        }
    }
    m_looping = false;
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_epoll->updateChannel(channel);
}