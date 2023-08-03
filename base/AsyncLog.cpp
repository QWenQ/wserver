#include "AsyncLog.h"
#include "LogFile.h"

#include <chrono>
#include <format>

AsyncLog::AsyncLog(const std::string& basename, int flush_interval) 
:   m_running(false),
    m_interval_sec(flush_interval),
    m_thread(std::bind(&AsyncLog::threadFunc, this)),
    m_basename(basename),
    m_latch(1),
    m_mutex(),
    m_cond(m_mutex),
    m_cur_buffer(new Buffer()),
    m_next_buffer(new Buffer()),
    m_buffers()
{ }

AsyncLog::~AsyncLog() {
    if (m_running == true) {
        stop(); 
    }
}

void AsyncLog::append(char* msg, size_t len) {
    MutexLockGuard lock(m_mutex);
    if (m_cur_buffer->available() >= len) {
        m_cur_buffer->append(msg, len);
    }
    else {
        m_buffers.push_back(std::move(m_cur_buffer));
        if (!m_next_buffer) {
            // if writing to log file is not over
            m_next_buffer.reset(new Buffer()); 
        }
        else {
            m_cur_buffer.swap(m_next_buffer);
        }
        m_cur_buffer->append(msg, len);
        m_cond.signal();
    }
}

void AsyncLog::threadFunc() {
    m_latch.countDown();
    LogFile output(m_basename);
    BufferPtr new_buffer_1(new Buffer());
    BufferPtr new_buffer_2(new Buffer());
    BufferVector write_to_file;
    while (m_running) {
        // get buffers ready to write to file
        {
            MutexLockGuard lock(m_mutex);
            if (m_buffers.empty()) {
                // todo: wait a specified time or until signaled
                m_cond.waitForSeconds(m_interval_sec);
            }
            m_buffers.push_back(std::move(m_cur_buffer));
            m_cur_buffer.swap(new_buffer_1);
            write_to_file.swap(m_buffers);
            if (!m_next_buffer) {
                m_next_buffer.swap(new_buffer_2);
            }
        } 

        // if buffers are full of trash
        if (write_to_file.size() > 20) {
            char buf[256] = {0};

            auto timestamp = std::chrono::high_resolution_clock::now();
            snprintf(buf, sizeof(buf), "Dropped log message at %s, %zd larger buffers\n",
                    std::format("{:%Y %m %d %H %M %S}", timestamp),
                    write_to_file.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            write_to_file.erase(write_to_file.begin() + 2, write_to_file.end()); 
        }

        // write buffers to logfile object
        for (const auto& buffer : write_to_file) {
            output.append(buffer->data(), buffer->length());
        }

        // clear redundant buffers
        if (write_to_file.size() > 2) {
            write_to_file.resize(2);
        }

        if (!new_buffer_1) {
            new_buffer_1.swap(write_to_file.back());
            write_to_file.pop_back();
            new_buffer_1.reset();
        }

        if (!new_buffer_2) {
            new_buffer_2.swap(write_to_file.back());
            write_to_file.pop_back();
            new_buffer_2.reset();
        }

        write_to_file.clear();
        // write data to file
        output.flush();
    }
    output.flush();
}