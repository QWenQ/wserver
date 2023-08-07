#include "CurrentThread.h"


namespace CurrentThread {
    __thread int t_cached_tid = 0;
    __thread char t_tid_string[32] = {0};
    __thread int t_tid_string_len = 6;

    void cacheTid() {
        if (t_cached_tid == 0) {
            t_cached_tid = ::gettid();
            t_tid_string_len = snprintf(t_tid_string, sizeof(t_tid_string), "%5d", t_cached_tid);
        }
    }
}