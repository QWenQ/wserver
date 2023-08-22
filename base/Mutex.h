#ifndef BASE_MUTEX_H
#define BASE_MUTEX_H

#include <pthread.h>
#include <assert.h>
#include <iostream>
#include "noncopyable.h"
#include "Logging.h"

class MutexLock : public noncopyable {
    public: 
        MutexLock() {
            int ret = pthread_mutex_init(&m_mutex, NULL);
            if (ret != 0) {
                LOG_FATAL << "MutexLock::MutexLock() error!";
            }
        }

        ~MutexLock() {
            int ret = pthread_mutex_destroy(&m_mutex);
            if (ret != 0) {
                LOG_FATAL << "MutexLock::~MutexLock() error!";
            }
        }

        // called only by MutexLockGuard
        int lock() {
            int ret = pthread_mutex_lock(&m_mutex);
            if (ret != 0) {
                printf("pthread_mutex_lock() error!\n");
            }
            return ret;
        }

        // called only by MutexLockGuard
        int unlock() {
            int ret = pthread_mutex_unlock(&m_mutex);
            if (ret != 0) {
                printf("pthread_mutex_unlock()\n");
            }
            return ret;
        }

        // called only by Condition
        pthread_mutex_t* getMutex() { return &m_mutex; }
    private:
        pthread_mutex_t m_mutex;
        // TODO: the holder who holding this mutex
}; // class MutexLock


class MutexLockGuard : public noncopyable {
    public: 
        explicit MutexLockGuard(MutexLock& mutex) : m_mutex(mutex) { m_mutex.lock(); }

        ~MutexLockGuard() { m_mutex.unlock(); }
    private: 
        MutexLock& m_mutex;
}; // class MutexLockGuard

#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")

#endif // BASE_MUTEX_H
