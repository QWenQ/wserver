#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include <unistd.h>
#include <stdio.h>

namespace CurrentThread {
    extern __thread int t_cached_tid;
    extern __thread char t_tid_string[32];
    extern __thread int t_tid_string_len;

    void cacheTid();

    inline int tid() {
        if (__builtin_expect(t_cached_tid == 0, 0)) {
            cacheTid();
        }
        return t_cached_tid;
    }

    inline const char* tidString() {
        return t_tid_string;
    }

    inline int tidStringLenght() {
        return t_tid_string_len;
    }

} // namespace CurrentThread
#endif // CURRENT_THREAD_H