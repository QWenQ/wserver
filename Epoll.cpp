#include <stdio.h>
#include <sys/epoll.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "Epoll.h"
#include "Channel.h"
#include "base/Logging.h"


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
        LOG_FATAL << "EpoLL::Epoll()";
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
        // nothing happened
    }
    else if (nums < 0) {
        if (errno != EINTR) {
            LOG_ERROR << "Epoll::poll";
        }
    }
}

void Epoll::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    if (static_cast<size_t>(numEvents) > m_events.size()) {
        LOG_ERROR << "Epoll::fillActiveChannels() error!";
    }
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(m_events[i].data.ptr);
        #ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it = m_channels.find(fd);
        if (it == m_channels.end() 
            || it->second != channel) {
            LOG_ERROR << "Epoll::fillActiveChannels() error!";
        }
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
            if (m_channels.find(fd) != m_channels.end()) {
                LOG_ERROR << "Epoll::updateChannel() error!";
            }
            m_channels[fd] = channel;
        }
        else if (index == kDeleted) {
            // reuse the connection deleted before
            if (m_channels.find(fd) == m_channels.end()
                || m_channels[fd] != channel) {
                    LOG_ERROR << "Epoll::updateChannel() error!";
                }
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else if (index == kAdded) {
        // the connection is being used in current thread loop
        int fd = channel->fd();
        if (m_channels.find(fd) == m_channels.end()
            || m_channels[fd] != channel) {
                LOG_ERROR << "Epoll::updateChannel() error!";
        }
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

void Epoll::removeChannel(Channel* channel) {
    // LOG_DEBUG << "assertInLoopThread()";
    assertInLoopThread();
    int fd = channel->fd();
    if (m_channels.find(fd) != m_channels.end()
        && m_channels[fd] == channel
        && channel->isNoneEvent()) {
        int index = channel->index();
        if (index == kAdded || index == kDeleted) {
            size_t n = m_channels.erase(fd);
            if (n != 1) {
                LOG_ERROR << "Epoll::removeChannel failed.";
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
    if (operation == EPOLL_CTL_DEL) {
        LOG_DEBUG << "remove fd " << fd << " from epoll " << m_epollfd;
    }
    if (ret == -1) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR << "epoll_ctl op = EPOLL_CTL_DEL fd = " << channel->fd(); 
        }
        else if (operation == EPOLL_CTL_ADD) {
            LOG_FATAL << "epoll_ctl op = EPOLL_CTL_ADD fd = " << channel->fd(); 
        }
        else if (operation == EPOLL_CTL_MOD) {
            LOG_FATAL << "epoll_ctl op = EPOLL_CTL_MOD fd = " << channel->fd(); 
        }
    }
}
