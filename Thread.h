#ifndef THREAD_H
#define THREAD_H

#include <vector>
#include <functional>
#include "Epoll.h"

class Thread {
    public:
        typedef std::function<void()> ThreadFunc;

        Thread();
        ~Thread();

        bool started() const { return m_started; }
        void start();

    private:
        bool m_started;
        pthread_t m_ptid;
        pid_t m_tid;
        ThreadFunc m_func;
}; // class Thread

#endif // THREAD_H