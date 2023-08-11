#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() 
:   m_thread(std::bind(&EventLoopThread::threadFunc, this)),
    m_loop(NULL),
    m_mutex(),
    m_cond(m_mutex)
{ }

EventLoopThread::~EventLoopThread() {
    if (m_loop != NULL) {
        m_loop->quit();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(m_thread.started() == false);
    // the start() method will call threadFunc() 
    // which will complete the assignment of m_loop.
    m_thread.start();
    {
        MutexLockGuard lock(m_mutex);
        // reject spurious wakeup
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
    m_loop = nullptr;
}