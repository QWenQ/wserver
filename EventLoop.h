#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <memory>
#include <functional>

#include "base/noncopyable.h"
#include "base/Mutex.h"

class Epoll;
class Channel;
class Timer;

class EventLoop : noncopyable {
    public:
        typedef std::function<void()> TimerCallBack;
        typedef std::function<void()> Functor;
        static const int kPollTimeoutMs;

        EventLoop();
        EventLoop(Epoll* epoll);
        ~EventLoop();

        void loop();

        void assertInLoopThread() {
            if (!isInLoopThread()) {
                abortNotInLoopThread();
            }
        }

        bool isInLoopThread() {
            return m_tid == gettid();
        }

        static EventLoop* getEventLoopOfCurrentThread();
        
        void updateChannel(Channel* channel);

        void quit() {
            m_quit = true;
            if (!isInLoopThread()) {
                wakeup();
            }
        }

        // used for timeout events
        void runAt(time_t delay, const TimerCallBack& cb);

        // for task assignment to I/O threads
        void runInLoop(const Functor& cb);
        void queueInLoop(const Functor& cb);
    private:
        void abortNotInLoopThread();
        // read one byte from m_wakeup_fd
        void handleRead();
        // handle pending events registered by the main thread
        void doPendingFunctors();
        // write one byte into m_wakeup_fd
        void wakeup();

        typedef std::vector<Channel*> ChannelList;

        bool m_looping;
        bool m_quit;
        bool m_calling_pending_functors;
        const pid_t m_tid;
        std::unique_ptr<Epoll> m_epoll;
        ChannelList m_active_channels;
        std::unique_ptr<TimerHeap> m_timer_heap;
        int m_wakeup_fd;
        // for readable events in m_wakeup_fd
        std::unique_ptr<Channel> m_wakeup_channel;
        // lock for pending functors
        MutexLock m_mutex;
        // allowed to be visited by the outside
        std::vector<Functor> m_pending_functors;
}; // class EventLoop   
#endif // EVENTLOOP_H