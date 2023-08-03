#include "LogFile.h"
#include "FileUtil.h"

LogFile::LogFile(const std::string& filename) 
:   m_basename(filename),
    m_mutex(),
    m_file(new AppendFile(filename))
{
    // todo
}

LogFile::~LogFile() = default;


void LogFile::append(const char* msg, size_t len) {
    MutexLockGuard lock(m_mutex);
    m_file->append(msg, len);
}

void LogFile::flush() {
    MutexLockGuard lock(m_mutex);
    m_file->flush();
}

