#include <pthread.h>
#include <sys/epoll.h>
#include <errno.h>
#include "Thread.h"
#include "HttpRequest.h"

Thread::Thread()
:   m_loop(true),
    m_epoll()
{
    int ret = pthread_create(&m_tid, nullptr, worker, this);
    if (ret == 0) {
        m_loop = false;
    }
    // when a detached thread terminates, its resources will
    // be automatically released back to the system without
    // another thread to join with the terminated thread.
    pthread_detach(m_tid);
}

Thread::~Thread() { }

void Thread::loop() {
    while (m_loop) {
       const std::vector<struct epoll_event>& events = m_epoll.epoll();

        // handle IO events
        for (int i = 0; i < events.size(); ++i) {
            if (events[i].events && EPOLLIN) {
                handleReadEvent(events[i].data.fd);
            }
            else if (events[i].events && EPOLLOUT) {
                handleWriteEvent(events[i].data.fd);
            }
            else {
                // todo: more events
            }
        }
    }
}


void* Thread::worker(void* arg) {
    Thread* p_thread = (Thread*)(arg);
    p_thread->loop();
    return p_thread;
}


void Thread::handleNewConnection(int fd) {
    // todo
}


void Thread::handleReadEvent(int fd) { 
    // todo
    // get http request from target fd
    // parse http request
    HttpRequest req = HttpRequest(fd);
    req.start();
}


void Thread::handleWriteEvent(int fd) { 
    // todo
}