#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <memory>

#include "Mutex.h"
#include "noncopyable.h"

class AppendFile;

class LogFile : noncopyable {
    public:
        LogFile(const std::string& filename, off_t roll_size, int flush_interval = 3, int check_every_N = 1024);
        ~LogFile();

        void append(const char* msg, size_t len);

        void flush();
    private:
        void append_unlocked(const char* msg, size_t len);
        // create a new log file with related attributes of LogFile object
        bool rollFile();
        static std::string getLogFileName(const std::string& basename, time_t* now);
        

        const std::string m_basename;
        const off_t m_roll_size;
        const time_t m_flush_interval;
        const int m_check_every_N;
        
        int m_cnt;

        std::unique_ptr<MutexLock> m_mutex;
        time_t m_start_of_period;
        time_t m_last_roll;
        time_t m_last_flush;
        std::unique_ptr<AppendFile> m_file;

        // roll file for every 24 hours
        static const int kRollPerSeconds = 60 * 60 * 24;
}; // class LogFile

#endif // LOG_FILE_H