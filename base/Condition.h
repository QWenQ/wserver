#ifndef BASE_CONDITION_H
#define BASE_CONDITION_H

#include <pthread.h>
#include <iostream>
#include "Mutex.h"
#include "noncopyable.h"

class Condition : public noncopyable {
    public: 
        Condition(MutexLock& mutex) : m_mutex(mutex) {
            assert(pthread_cond_init(&m_cond, NULL) == 0); 
        }

        ~Condition() {
            assert(pthread_cond_destroy(&m_cond) == 0);
        }

        int wait() {
            int ret = pthread_cond_wait(&m_cond, m_mutex.getMutex());
            if (ret != 0) {
                printf("pthread_cond_wait() error!\n");
            }
            return ret;
        }

        int signal() {
            int ret = pthread_cond_signal(&m_cond);
            if (ret != 0) {
                printf("pthread_cond_signal() error!\n");
            }
            return ret;
        }

        int broadcast() {
            int ret = pthread_cond_broadcast(&m_cond);
            if (ret != 0) {
                printf("pthread_cond_broadcast() error!\n");
            }
            return ret;
        }

    private: 
        pthread_cond_t m_cond;
        MutexLock& m_mutex;
}; // class Condition

#endif // BASE_CONDITION_H
