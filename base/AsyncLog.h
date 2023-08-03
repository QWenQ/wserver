#ifndef ASYNCLOG_H
#define ASYNCLOG_H

#include <vector>
#include <memory>

#include "noncopyable.h"
#include "Mutex.h"
#include "Condition.h"
#include "../Thread.h"
#include "LogStream.h"

class AsyncLog : noncopyable {
    public:
        AsyncLog(const std::string& basename, int flush_interval);
        ~AsyncLog();

        void start() {
            m_running = true;
            m_thread.start();
            // main thread wait until the log thread starts working
            m_latch.wait();
        }
        void stop() {
            m_running = false;
            // notity the log thread to write all data left in buffers to the log file immediately.
            m_cond.signal();
        }

        // called by the front end
        void append(char* msg, size_t len);
    private:
        // used by the back end
        void threadFunc();


        typedef FixedBuffer<kLargerBufferSize> Buffer;
        typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
        typedef BufferVector::value_type BufferPtr;

        bool m_running;
        const int m_interval_sec;
        Thread m_thread;
        const std::string m_basename;
        CountDownLatch m_latch;
        MutexLock m_mutex;
        Condition m_cond;
        BufferPtr m_cur_buffer;
        BufferPtr m_next_buffer;
        BufferVector m_buffers;
}; // class AsyncLog

#endif // ASYNCLOG_H