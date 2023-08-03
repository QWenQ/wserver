#ifndef TIMER_HEAP_H
#define TIMER_HEAP_H

#include <vector>
#include "Channel.h"

class Timer;
class EventLoop;

class TimerHeap {
    public:
        typedef std::vector<std::unique_ptr<Timer>> TimerList;
        typedef std::function<void()> CallBack;

        TimerHeap(EventLoop* loop);
        ~TimerHeap() = default;

        void addTimer(CallBack cb, time_t delay);
        const std::unique_ptr<Timer>& top() const;
        void pop();

        void handleRead();
        
    private:
        void percolateDown(int index);
        void percolateUp(int index);

        void addTimerInLoop(CallBack cb, time_t delay);

        EventLoop* m_ownerloop;
        const int m_timerfd;
        // used to wait readable event in m_timerfd
        Channel m_timer_channel;

        TimerList m_min_heap;
}; // class TimerHeap
#endif // TIMER_HEAP_H