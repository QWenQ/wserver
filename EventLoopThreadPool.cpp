#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, int thread_nums) 
:   m_base_loop(loop),
    m_started(false),
    m_thread_nums(thread_nums),
    m_next(0),
    m_threads(),
    m_loops()
{ }

EventLoopThreadPool::~EventLoopThreadPool() { }

void EventLoopThreadPool::start() {
    m_base_loop->assertInLoopThread();
    m_started = true;
    for (int i = 0; i < m_thread_nums; ++i) {
        EventLoopThread* t = new EventLoopThread();
        m_threads.push_back(std::unique_ptr<EventLoopThread>(t));
        m_loops.push_back(t->startLoop()); 
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    m_base_loop->assertInLoopThread();
    // round-robin
    EventLoop* loop = m_loops[m_next];
    m_next = (m_next + 1) % m_loops.size();
    return loop;
}