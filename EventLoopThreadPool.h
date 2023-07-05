#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>

#include "base/noncopyable.h"

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable {
    public:
        EventLoopThreadPool(EventLoop* loop, int thread_nums);
        ~EventLoopThreadPool();
        void start();
        void setThreadNums(int nums) { m_thread_nums = nums; }
        EventLoop* getNextLoop();
    private:
        EventLoop* m_base_loop;
        bool m_started;
        int m_thread_nums;
        int m_next;
        std::vector<std::unique_ptr<EventLoopThread>> m_threads;
        std::vector<EventLoop*> m_loops;
}; // class EventLoopThreadPool
#endif // EVENTLOOPTHREADPOOL_H