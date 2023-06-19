#ifndef EVENT_LOOP_THREAD_H
#define EVENT_LOOP_THREAD_H

#include "EventLoop.h"
#include "Thread.h"
#include "base/Condition.h"

class EventLoopThread {
    public:
        EventLoopThread();
        ~EventLoopThread();

        EventLoop* startLoop();
        void threadFunc();

    private:
        Thread m_thread;
        EventLoop* m_loop; 
        MutexLock m_mutex;
        Condition m_cond;

}; // class EventLoopThread
#endif // EVENT_LOOP_THREAD_H