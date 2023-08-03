#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <memory>

#include "Mutex.h"

class AppendFile;

class LogFile {
    public:
        LogFile(const std::string& filename);
        ~LogFile();

        void append(const char* msg, size_t len);

        void flush();
    private:
        // todo
        const std::string m_basename;

        MutexLock m_mutex;
        std::unique_ptr<AppendFile> m_file;
}; // class LogFile

#endif // LOG_FILE_H