#include <pthread.h>
#include <sys/epoll.h>
#include <errno.h>
#include "Thread.h"
#include "HttpRequest.h"

Thread::Thread() {
    // todo
}

Thread::~Thread() { 
    // todo
}

void Thread::start() {
    // todo: calling back?
    m_func();
}