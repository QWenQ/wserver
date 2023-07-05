#ifndef BASE_COUNTDOWNLATCH_H
#define BASE_COUNTDOWNLATCH_H

#include "noncopyable.h"
#include "Condition.h"
#include "Mutex.h"

class CountDownLatch : public noncopyable {
    public:
        explicit CountDownLatch(int count);
        void wait();
        void countDown();
        int getCount() const;
    private:
        mutable MutexLock mutex_;
        Condition condition_;
        int count_;
}; // class CountDownLatch
#endif // BASE_COUNTDOWNLATCH_
