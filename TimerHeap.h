#ifndef TIMER_HEAP_H
#define TIMER_HEAP_H

#include <vector>
#include "Channel.h"

class Timer;
class EventLoop;

class TimerHeap {
    public:
        typedef std::vector<Timer> TimerList;

        TimerHeap();
        ~TimerHeap() = default;

        void addTimer(Timer timer);
        const Timer& top() const;
        void removeTimer();
        // void tick();

        TimerList getExpired();
        
    private:
        void percolateDown(int index);
        void percolateUp(int index);

        void addTimerInLoop(Timer& timer);

        EventLoop* m_ownerloop;
        const int m_timerfd;
        // used to wait readable event in m_timerfd
        Channel m_timer_channel;

        TimerList m_min_heap;
}; // class TimerHeap
#endif // TIMER_HEAP_H