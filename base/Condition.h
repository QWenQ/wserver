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

        bool waitForSeconds(int seconds) { 
            timespec abstime;
            clock_gettime(CLOCK_MONOTONIC, &abstime);
            const int64_t kNanoSecondsPerSecond = 1000000000;
            int64_t nano_seconds = seconds * kNanoSecondsPerSecond;
            abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nano_seconds) / kNanoSecondsPerSecond);
            abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nano_seconds) % kNanoSecondsPerSecond);
            return ETIMEDOUT == pthread_cond_timedwait(&m_cond, m_mutex.getMutex(), &abstime);
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
