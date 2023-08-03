#include <sys/timerfd.h>

#include "TimerHeap.h"
#include "Timer.h"

TimerHeap::TimerHeap(EventLoop* loop)
:   m_ownerloop(loop),
    m_timerfd(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK)),
    m_timer_channel(loop, m_timerfd),
    m_min_heap()
{
    m_timer_channel.setReadHandler(std::bind(&TimerHeap::handleRead, this));
    m_timer_channel.enableReading();
}


void TimerHeap::percolateDown(int index) {
    if (index >= m_min_heap.size()) return;
    int left = 2 * index + 1;
    int right = left + 1;
    int next = index;
    if (left < m_min_heap.size() && m_min_heap[left]->less(m_min_heap[next])) {
        next = left;
    }

    if (right < m_min_heap.size() && m_min_heap[right]->less(m_min_heap[next])) {
        next = right;
    }

    if (next != index) {
        m_min_heap[index].swap(m_min_heap[next]);
        percolateDown(next);
    }
}

void TimerHeap::percolateUp(int index) {
    if (index == 0) return;
    int parent = (index - 1) >> 1;
    if (m_min_heap[index]->less(m_min_heap[parent])) {
        m_min_heap[index].swap(m_min_heap[parent]);
        percolateUp(parent);
    }
}

void TimerHeap::addTimer(TimerHeap::CallBack cb, time_t delay) {
    m_ownerloop->runInLoop(std::bind(&TimerHeap::addTimerInLoop, this, cb, delay));
}

void TimerHeap::addTimerInLoop(TimerHeap::CallBack cb, time_t delay) {
    std::unique_ptr<Timer> timer(new Timer(delay, std::move(cb)));
    m_min_heap.push_back(std::move(timer));
    percolateUp(m_min_heap.size() - 1);
}

void TimerHeap::pop() {
    // release and delete the owned Timer
    m_min_heap[0].reset(nullptr);
    int last = m_min_heap.size() - 1;
    std::swap(m_min_heap[0], m_min_heap[last]);
    m_min_heap.pop_back();
    percolateDown(0);
}

const std::unique_ptr<Timer>& TimerHeap::top() const {
    return m_min_heap.front();
}


void TimerHeap::handleRead() {
    m_ownerloop->assertInLoopThread();
    time_t cur = time(NULL);
    while (!m_min_heap.empty()) {
        const std::unique_ptr<Timer>& timer = m_min_heap.front();
        if (timer->getExpireTime() >= cur) {
            break;
        }
        timer->handleTimeoutEvent();
        pop();
    }
}
