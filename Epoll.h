#ifndef EPOLL_H
#define EPOLL_H

#include <vector>
#include <map>
#include "base/noncopyable.h"

class Channel;
class EventLoop;

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

class Epoll : noncopyable {
    public:
        typedef std::vector<Channel*> ChannelList;

        Epoll(EventLoop* loop);
        ~Epoll();

        const static int MAXEVENTS;
        const static int TIMEOUT;
        const static int kInitEventListSize;

        // polls the I/O events.
        // must be called in the loop thread
        void poll(int timeoutMs, ChannelList* activeChannels);

        // change the interested I/O events
        // must be called in the loop thread
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);

        void assertInLoopThread() {
            m_owner_loop->assertInLoopThread();
        }


    private:

        void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

        void update(int operation, Channel* channel);  

        typedef std::vector<struct epoll_event> EventList;
        // map fd to Channel*
        typedef std::map<int, Channel*> ChannelMap;
        
        EventLoop* m_owner_loop;
        int m_epollfd;
        // struct epoll_event -> epoll_data -> ptr = Channel*
        EventList m_events;
        ChannelMap m_channels;
}; // class Epoll

#endif // EPOLL_H