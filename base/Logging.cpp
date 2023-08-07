#include "Logging.h"
#include "CurrentThread.h"
#include "timeUtils.h"

#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>


const char* log_level_name[]{ "INFO", "WARN", "ERROR" };

// init g_log_level
Logger::LogLevel g_loglevel = Logger::INFO;

Logger::Impl::Impl(Logger::LogLevel level, const std::string& filename, int line)
:   m_stream(),
    m_level(level),
    m_line(line),
    m_basename(filename)
{ 
    formatTime();
    // collect thread ID in Linux OS and log level
    // CurrentThread::cacheTid();
    m_stream << CurrentThread::tidString() << " " << log_level_name[m_level] << " ";
}

void Logger::Impl::finish() {
    m_stream << " - "  << m_basename << ":" << m_line << '\n';
}

void Logger::Impl::formatTime() {
    std::string time_stamp = timeStamp();
    
    m_stream << time_stamp << " ";
}

Logger::Logger(const std::string& basename, int line, Logger::LogLevel level) 
:   m_impl(level, basename, line)
{ }

void defaultOutputFunc(const char* msg, size_t len) {
    fwrite(msg, 1, len, stdout);
}

void defaultFlushFunc() {
    ::fflush(stdout);
}

Logger::outputFunc g_output = defaultOutputFunc;
Logger::flushFunc g_flush = defaultFlushFunc;

Logger::~Logger() {
    // before destruction of Logger object,
    // write data left in the buffer of m_impl.m_stream to the asyncLog thread
    m_impl.finish();
    g_output(stream().buffer(), stream().size());
    if (m_impl.m_level == Logger::ERROR) {
        g_flush();
        abort();
    }
}

void Logger::setOutputFunc(Logger::outputFunc out) {
    g_output = out;
}

void Logger::setFlushFunc(Logger::flushFunc flush) {
    g_flush = flush;
}

