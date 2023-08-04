#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include <unistd.h>
#include <stdio.h>

namespace CurrentThread {
extern __thread int t_cached_tid;
extern __thread char t_tid_string[32];
extern __thread int t_tid_string_len;

__thread int t_cached_tid = 0;
__thread char t_tid_string[32] = {0};
__thread int t_tid_string_len = 6;

void cacheTid() {
    if (t_cached_tid == 0) {
        t_cached_tid = ::gettid();
        t_tid_string_len = snprintf(t_tid_string, sizeof(t_tid_string), "%5d", t_cached_tid);
    }
}

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