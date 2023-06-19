#include <assert.h>
#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "Timer.h"
#include "TimerHeap.h"

// the thread variable is an independent entity for each thread
// and the variables of thread do not interfere with each other.
__thread EventLoop* t_loopInThisThread = 0;

const int EventLoop::kPollTimeoutMs = 5000;

EventLoop::EventLoop() {
    // todo
}

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
        // handle I/O events
        for (Channel* it : m_active_channels) {
            it->handleEvent();
        }

        // handle timeout events ?? so, what's m_timerfd in class TimerHeap for?
        std::vector<Timer> timeout_events = m_timer_heap->getExpired();
        for (auto it : timeout_events) {
            it.handleTimeoutEvent();
        }

        doPendingFunctors();
    }
    m_looping = false;
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    m_epoll->updateChannel(channel);
}


void EventLoop::runAt(time_t delay, const TimerCallBack& cb) {
    m_timer_heap->addTimer(Timer(delay, cb));
}

void EventLoop::runInLoop(const Functor& cb) {
    if (isInLoopThread()) {
        // if the loop is in its owner thread, call back cb()
        cb();
    }
    else {
        // or add it to a queue to wake loop's owner thread up to call the cb()
       queueInLoop(cb); 
    }
}

void EventLoop::queueInLoop(const Functor& cb) {
    {
        MutexLockGuard lock(m_mutex);
        m_pending_functors.push_back(cb);
    }
    if (!isInLoopThread() || m_calling_pending_functors) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    m_calling_pending_functors = true;
    {
        MutexLockGuard lock(m_mutex);
        functors.swap(m_pending_functors);
    }

    for (auto& func : functors) {
        func();
    }
    m_calling_pending_functors = false;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        // error handling
        perror("wake up failed!");
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(m_wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        // error handling
        perror("handle read for wake up failed!");
    }
}