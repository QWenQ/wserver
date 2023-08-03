#include "FileUtil.h"


AppendFile::AppendFile(std::string filename)
:   m_fp(::fopen(filename.c_str(), "ae")),
    m_writed_bytes(0)
{
    ::setbuffer(m_fp, m_buffer, sizeof m_buffer);
}

AppendFile::~AppendFile() {
    ::fclose(m_fp);
}

void AppendFile::append(const char* msg, size_t len) {
    size_t written = 0;
    while (written < len) {
        size_t rest = len - written;
        size_t ret = write(msg + written, rest);
        if (ret != rest) {
            int err = ferror(m_fp);
            if (err) {
                fprintf(stderr, "AppenFile::append() failed %s\n", strerror_tl(err));
                break;
            }
        }
        written += ret;
    }
    m_writed_bytes += written;
}

void AppendFile::flush() {
    ::fflush(m_fp);
}

size_t AppendFile::write(const char* msg, size_t len) {
    return ::fwrite_unlocked(msg, 1, len, m_fp);
}