#ifndef THREAD_H
#define THREAD_H

#include <vector>
#include "Epoll.h"

class Thread {
    public:
        Thread();
        ~Thread();

        void loop();


        void handleNewConnection(int fd);
        void handleReadEvent(int fd);
        void handleWriteEvent(int fd);

        static void* worker(void* arg);
    private:
        const static int MAX_EVENTS = 1024;
        const static int TIMEOUT = 5000;

        pthread_t m_tid;
        bool m_loop;
        Epoll m_epoll;
}; // class Thread

#endif // THREAD_H