#include "TimerHeap.h"
#include "Timer.h"

TimerHeap::TimerHeap()
:   m_min_heap() 
{ 
    // todo 
}


void TimerHeap::percolateDown(int index) {
    if (index >= m_min_heap.size()) return;
    int left = 2 * index + 1;
    int right = left + 1;
    int next = index;
    if (left < m_min_heap.size() && m_min_heap[left] < m_min_heap[next]) {
        next = left;
    }

    if (right < m_min_heap.size() && m_min_heap[right] < m_min_heap[next]) {
        next = right;
    }

    if (next != index) {
        std::swap(m_min_heap[index], m_min_heap[next]);
        percolateDown(next);
    }
}

void TimerHeap::percolateUp(int index) {
    if (index == 0) return;
    int parent = (index - 1) >> 1;
    if (m_min_heap[index] < m_min_heap[parent]) {
        std::swap(m_min_heap[index], m_min_heap[parent]);
        percolateUp(parent);
    }
}

void TimerHeap::addTimer(Timer timer) {
    m_ownerloop->runInLoop(std::bind(addTimerInLoop, this, timer));
}

void TimerHeap::addTimerInLoop(Timer& timer) {
    m_ownerloop->assertInLoopThread();
    m_min_heap.push_back(std::move(timer));
    percolateUp(m_min_heap.size() - 1);
}

void TimerHeap::removeTimer() {
    int last = m_min_heap.size() - 1;
    std::swap(m_min_heap[0], m_min_heap[last]);
    m_min_heap.pop_back();
    percolateDown(0);
}

const Timer& TimerHeap::top() const {
    return m_min_heap.front();
}

TimerHeap::TimerList TimerHeap::getExpired() {
    TimerList ret;
    time_t cur = time(NULL);
    while (!m_min_heap.empty()) {
        Timer timer = m_min_heap.front();
        if (timer.getExpireTime() >= cur) {
            break;
        }
        ret.push_back(timer);
        removeTimer();
    }
    return ret;
}


