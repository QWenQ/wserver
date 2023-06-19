#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() {
    // todo
}

EventLoopThread::~EventLoopThread() {
    // todo
}

EventLoop* EventLoopThread::startLoop() {
    assert(m_thread.started());
    m_thread.start();
    {
        MutexLockGuard lock(m_mutex);
        while (m_loop == NULL) {
            m_cond.wait();
        }
    }

    return m_loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(m_mutex);
        m_loop = &loop;
        m_cond.signal();
    }

    loop.loop();
}