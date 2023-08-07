#include "LogFile.h"
#include "FileUtil.h"

#include <string>
#include <ctime>
#include <unistd.h>

LogFile::LogFile(const std::string& basename,
                off_t roll_size,
                int flush_interval,
                int check_every_N)
:   m_basename(basename),
    m_roll_size(roll_size),
    m_flush_interval(flush_interval),
    m_check_every_N(check_every_N),
    m_cnt(0),
    m_mutex(new MutexLock()),
    m_start_of_period(0),
    m_last_roll(0),
    m_last_flush(0)    
{
    rollFile();
}

LogFile::~LogFile() = default;


void LogFile::append(const char* msg, size_t len) {
    if (m_mutex) {
        MutexLockGuard lock(*m_mutex);
        append_unlocked(msg, len);
    }
    else {
        append_unlocked(msg, len);
    }
}

void LogFile::flush() {
    if (m_mutex) {
        MutexLockGuard lock(*m_mutex);
        m_file->flush();
    }
    else {
        m_file->flush();
    }
}


void LogFile::append_unlocked(const char* msg, size_t len) {
    // append the message the m_file member
    m_file->append(msg, len);

    // roll file if log file space is not enough
    if (m_file->writtenBytes() > m_roll_size) {
        rollFile();
    }
    else {
        ++m_cnt;
        if (m_cnt >= m_check_every_N) {
            m_cnt = 0;
            // roll file every 24 hours
            time_t now = ::time(NULL);
            time_t current_period = now / kRollPerSeconds * kRollPerSeconds;
            if (current_period > m_start_of_period) {
                rollFile();
            }
            // flush if flush time interval is over
            else if (now - m_last_flush > m_flush_interval) {
                m_last_flush = now;
                m_file->flush();
            }
        }
    }
}


bool LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(m_basename, &now);
    time_t start = now / kRollPerSeconds * kRollPerSeconds;

    if (now > m_last_roll) {
        m_last_roll = now;
        m_last_flush = now;
        m_start_of_period = start;
        m_file.reset(new AppendFile(filename));
        return true;
    } 
    return false;
}

/* format of log file name:
basename + year + month + day + hour + minute + second + hostname + pid + ".log"
*/
std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
    std::string filename;
    filename += basename;
    *now = std::time(NULL);
    char now_str[32];
    std::strftime(now_str, sizeof(now_str), ".%Y%m%d-%H%M%S.", std::localtime(now));
    filename += now_str;

    // POSIX HOSTNAME MAX SIZE 255
    char hostname[256];
    ::gethostname(hostname, sizeof(hostname));
    hostname[255] = '\0';
    filename += hostname;

    char pid_str[32];
    pid_t pid = ::getpid();
    snprintf(pid_str, sizeof(pid_str), ".%d", pid);
    filename += pid_str;

    filename += ".log";

    return filename;
}
