#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <memory>

#include "base/noncopyable.h"

class Epoll;
class Channel;

class EventLoop : noncopyable {
    public:
        static const int kPollTimeoutMs;

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

        void quit() { m_quit = true; }
    private:
        void abortNotInLoopThread();

        typedef std::vector<Channel*> ChannelList;

        bool m_looping;
        bool m_quit;
        const pid_t m_tid;
        std::unique_ptr<Epoll> m_epoll;
        ChannelList m_active_channels;
}; // class EventLoop   
#endif // EVENTLOOP_H