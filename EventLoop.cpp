#include <sys/eventfd.h>
#include <assert.h>
#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include "Timer.h"
#include "TimerHeap.h"
#include "base/Logging.h"

// the thread variable is an independent entity for each thread
// and the variables of thread do not interfere with each other.
__thread EventLoop* t_loopInThisThread = 0;

const int EventLoop::kPollTimeoutMs = 30000;

EventLoop::EventLoop() 
:   m_looping(false),
    m_quit(false),
    m_calling_pending_functors(false),
    m_event_handling(false),
    m_tid(CurrentThread::tid()),
    m_epoll(new Epoll(this)),
    m_active_channels(),
    m_current_active_channel(nullptr),
    m_timer_heap(new TimerHeap(this)),
    m_wakeup_fd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)),
    m_wakeup_channel(new Channel(this, m_wakeup_fd)),
    m_mutex(),
    m_pending_functors()
{
    if (m_wakeup_fd < 0) {
        LOG_FATAL << "Failed in eventfd";
    }

    LOG_INFO << "EventLoop created " << this << " in thread " << m_tid;

    if (t_loopInThisThread) {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                    << " exists in this thread " << m_tid;
    }
    else {
        t_loopInThisThread = this;
    }
    m_wakeup_channel->setReadHandler(std::bind(&EventLoop::handleRead, this));
    m_wakeup_channel->enableReading();
}


EventLoop::~EventLoop() {
    LOG_INFO << "EventLoop " << this << " of thread " << m_tid
                << " destructs in thread " << CurrentThread::tid();
    m_wakeup_channel->disableAll();
    m_wakeup_channel->remove();
    ::close(m_wakeup_fd);
    t_loopInThisThread = NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread() {
    return t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread() {
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
                << " was created in thread " << m_tid 
                << ", current thread id is " << CurrentThread::tid();
}


void EventLoop::loop() {
    if (m_looping) {
        LOG_ERROR << "EventLoop::loop() error!";
    }
    // LOG_DEBUG << "assertInLoopThread()";
    assertInLoopThread();
    m_looping = true;
    m_quit = false;
    while (!m_quit) {
        m_active_channels.clear();
        m_epoll->poll(kPollTimeoutMs, &m_active_channels);
        // handle I/O events
        m_event_handling = true;
        for (Channel* it : m_active_channels) {
            m_current_active_channel = it;
            m_current_active_channel->handleEvent();
        }
        m_current_active_channel = NULL;
        m_event_handling = false;
        doPendingFunctors();
    }
    m_looping = false;
}

void EventLoop::updateChannel(Channel* channel) {
    if (channel->ownerLoop() != this) {
        LOG_FATAL << "EventLoop::updateChannel() error!";
    }
    // LOG_DEBUG << "assertInLoopThread() begin";
    assertInLoopThread();
    m_epoll->updateChannel(channel);
}


void EventLoop::runAfter(time_t delay, const TimerCallBack& cb) {
    m_timer_heap->addTimer(std::move(cb), delay);
}

void EventLoop::runInLoop(Functor cb) {
    if (isInLoopThread()) {
        // if the loop is in its owner thread, call back cb()
        cb();
    }
    else {
        // or add it to a queue to wake loop's owner thread up to call the cb()
       queueInLoop(std::move(cb)); 
    }
}

void EventLoop::queueInLoop(Functor cb) {
    LOG_DEBUG << "EventLoop::queueInLoop() begin";
    {
        MutexLockGuard lock(m_mutex);
        m_pending_functors.push_back(std::move(cb));
    }
    if (!isInLoopThread() || m_calling_pending_functors) {
        LOG_DEBUG << "wake up in EventLoop::queueInLoop() in EventLoop " << this;
        wakeup();
    }
    LOG_DEBUG << "EventLoop::queueInLoop() end";
}

void EventLoop::removeChannel(Channel* channel) {
    if (channel->ownerLoop() != this) {
        LOG_FATAL << "EventLoop::removeChannel() error!";
    }
    // LOG_DEBUG << "assertInLoopThread() begin";
    assertInLoopThread();
    if (m_event_handling) {
        LOG_DEBUG << "to be removed channel: " << channel << ", current active channel: " << m_current_active_channel;
        if (channel != m_current_active_channel
        || std::find(m_active_channels.begin(), m_active_channels.end(), channel) != m_active_channels.end()) {
            LOG_FATAL << "EventLoop::removeChannel() error!";
        }
    }
    m_epoll->removeChannel(channel);
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    m_calling_pending_functors = true;
    {
        MutexLockGuard lock(m_mutex);
        functors.swap(m_pending_functors);
    }

    for (auto& func : functors) {
        func();
    }
    m_calling_pending_functors = false;
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(m_wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::wakeup() writes" << n << " bytes instead of " << sizeof(one);
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(m_wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of " << sizeof(one);
    }
}