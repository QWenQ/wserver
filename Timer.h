#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <time.h>


class Timer {
    public:
        typedef std::function<void()> CallBack;

        Timer(time_t delay, CallBack cb)
        :   m_expire_time(::time(NULL) + delay),
            m_cb(std::move(cb))
        { }

        ~Timer() = default;

        time_t getExpireTime() const { return m_expire_time; }

        void handleTimeoutEvent() const { m_cb(); }

        bool operator<(const Timer& timer) { return m_expire_time < timer.m_expire_time; }

    private:
        // absolute time
        time_t m_expire_time;
        CallBack m_cb;
}; // class Timer
#endif // TIMER_H