#include <iostream>

#include "../EventLoop.h"
#include "../EventLoopThread.h"
#include "../TimerHeap.h"
#include "../base/timeUtils.h"

int cnt = 0;
EventLoop* g_loop = NULL;

void printTid() {
    printf("pid = %d, tid = %d\n", ::getpid(), CurrentThread::tid());
    printf("now %s\n", timeStamp().c_str());
}

void print(const char* msg) {
    printf("msg %s %s\n", timeStamp().c_str(), msg);
    ++cnt;
    if (cnt > 6) {
        g_loop->quit();
    }
}

int main() {
    printTid();
    sleep(1);
    {
        EventLoop loop;
        g_loop = &loop;
        print("main");
        loop.runAfter(1, std::bind(print, "once1"));
        loop.runAfter(2, std::bind(print, "once2"));
        loop.runAfter(3, std::bind(print, "once3"));
        loop.runAfter(4, std::bind(print, "once4"));
        loop.runAfter(5, std::bind(print, "once5"));
        loop.runAfter(6, std::bind(print, "once6"));
        loop.runAfter(7, std::bind(print, "once7"));
        loop.loop();
        print("main loop exits");
    }
    sleep(1);
    {
        EventLoopThread loop_thread;
        EventLoop* loop = loop_thread.startLoop();
        loop->runAfter(3, printTid);
        sleep(3);
        print("thread loop exits");
    }
    return 0;
}