#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "Epoll.h"
#include "Channel.h"


const int Epoll::MAXEVENTS = 10000;
const int Epoll::TIMEOUT = 5000;
const int Epoll::kInitEventListSize = 16;


Epoll::Epoll(EventLoop* loop)
:   m_owner_loop(loop),
    m_epollfd(::epoll_create(5)),
    m_events(kInitEventListSize),
    m_channels() 
{
    if (m_epollfd < 0) {
        perror("epoll_create() error!");
    }
}

Epoll::~Epoll() { 
    ::close(m_epollfd);
}

void Epoll::poll(int timeoutMs, ChannelList* activeChannels) {
    int nums = ::epoll_wait(m_epollfd, 
                            &*m_events.begin(), 
                            static_cast<int>(m_events.size()), 
                            timeoutMs); 
    
    if (nums > 0) {
        fillActiveChannels(nums, activeChannels);
        if (static_cast<size_t>(nums) == m_events.size()) {
            m_events.resize(m_events.size() * 2);
        }
    }
    else if (nums == 0) {
        // log: nothing happened
    }
    else if (nums < 0) {
        // error handling
        if (errno != EINTR) {
            perror("epoll_wait() error!");
        }
    }
}

void Epoll::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    assert(static_cast<size_t>(numEvents) <= m_events.size());
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(m_events[i].data.ptr);
        #ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it = m_channels.find(fd);
        assert(it != m_channels.end());
        assert(it->second == channel);
        #endif // NDEBUG
        channel->setRevents(m_events[i].events);
        activeChannels->push_back(channel);
    }
}

void Epoll::updateChannel(Channel* channel) {
    int index = channel->index();
    if (index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if (index == kNew) {
            // a new connection, EPOLL_CTL_ADD operation
            assert(m_channels.find(fd) == m_channels.end());
            m_channels[fd] = channel;
        }
        else if (index == kDeleted) {
            // reuse the connection deleted before
            assert(m_channels.find(fd) != m_channels.end());
            assert(m_channels[fd] == channel);
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else if (index == kAdded) {
        // the connection is being used in current thread loop
        int fd = channel->fd();
        assert(m_channels.find(fd) != m_channels.end());
        assert(m_channels[fd] == channel);
        if (channel->isNoneEvent()) {
            // no interested I/O events, remove the fd from epoll table
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } 
        else {
            // modifify
            update(EPOLL_CTL_MOD, channel);
        }
    } 
}

/*
void Epoll::removeChannel(Channel* channel) {
    assertInLoopThread();
    int fd = channel->fd();
    assert(m_channels.find(fd) != m_channels.end());
    assert(m_channels[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    size_t n = m_channels.erase(fd);
    assert(n == 1);
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}
*/

void Epoll::removeChannel(Channel* channel) {
    assertInLoopThread();
    int fd = channel->fd();
    if (m_channels.find(fd) != m_channels.end()
        && m_channels[fd] == channel
        && channel->isNoneEvent()) {
        int index = channel->index();
        if (index == kAdded || index == kDeleted) {
            size_t n = m_channels.erase(fd);
            if (n != 1) {
                perror("std::map::erase() error!");
            }
            if (index == kAdded) {
                update(EPOLL_CTL_DEL, channel);
            }
            channel->setIndex(kNew);
        }
    }
}

void Epoll::update(int operation, Channel* channel) {
    int fd = channel->fd();
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.ptr = channel;
    event.events = channel->events();
    int ret = ::epoll_ctl(m_epollfd, operation, fd, &event);
    if (ret == -1) {
        perror("epoll_ctl() error!");
    }
}
