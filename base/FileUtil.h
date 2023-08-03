#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <sys/types.h>
#include <stdio.h>
#include <string>

#include "noncopyable.h"

class AppendFile : noncopyable {
    public:
        AppendFile(std::string filename);
        ~AppendFile();

        void append(const char* msg, size_t len);

        void flush();
    private:
        size_t write(const char* msg, size_t len);

        FILE* m_fp;
        char m_buffer[64 * 1024];
        off_t m_writed_bytes;
}; // class AppendFile

#endif // FILE_UTIL_H