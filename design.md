support:
1. Reactor: non-blocking IO + IO multiplexing + threadpool
2. finite state machine to deal with HTTP request
3. timer: close timeout connection based on min heap
4. log: how?

nonblocking-IO + IO multiplexing + one loop per thread + threadpool

Threads in the server:
1. IO thread: listen to the new socket and get the new sockets to the idle threads
2. logical thread: deal the many requests from different clients

The server starts and listens to the new connections on the web.
When a request of connection is comming, the server will get it to the IO thread(main thread) by registering read event in the epoll of compute threads.
The blocked compute thread gets events from their own epoll and starts to deal with the request of client sockets.
timer: when 


## Classes and Data Structures
### class HttpRequest
support:
1. parse HTTP request


### class Buffer
### class Thread
Task:
1. handle new connections
2. handle read events
3. handle write events

### class Epoll
Task:
1. get events from epoll

### class ThreadPool

## Algorithms