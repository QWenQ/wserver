#include <pthread.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "Thread.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"

struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc m_func;
    pid_t* m_tid;
    CountDownLatch* m_latch;

    ThreadData(ThreadFunc func, pid_t* tid, CountDownLatch* latch)
    :   m_func(std::move(func)),
        m_tid(tid),
        m_latch(latch)
    { }

    void runInThread() {
        *m_tid = CurrentThread::tid();
        m_tid = NULL;
        // for what?
        // for child thread get its tid by calling ::gettid()
        // and then main thread continue work
        m_latch->countDown();
        m_latch = nullptr;

        try {
            m_func();
        }
        catch (const std::exception& ex) {
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
    }
}; // struct ThreadData

Thread::Thread(ThreadFunc func) 
:   m_started(false),
    m_ptid(0),
    m_tid(0),
    m_func(std::move(func)),
    m_latch(1)
{ }

Thread::~Thread() { }

void* startThread(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    data->runInThread();
    delete data;
    data = NULL;
    return data;
}

void Thread::start() {
    m_started = true;
    ThreadData* data = new ThreadData(m_func, &m_tid, &m_latch);
    int ret = pthread_create(&m_ptid, NULL, &startThread, data);
    if (ret != 0) {
        LOG_FATAL << "pthread_create() failed!";
    }
    // the resources are automatically released back to the system
    // without the need for another thread to join with the terminated thread.
    pthread_detach(m_ptid);
    // wait until the thread gets its tid
    m_latch.wait();
    if (m_tid <= 0) {
        LOG_FATAL << "Thread::start() error!";
    }
}