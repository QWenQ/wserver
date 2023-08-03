#include "Logging.h"
#include "CurrentThread.h"

#include <chrono>
// #include <format>

const char* log_level_name[]{ "INFO", "WARN", "ERROR" };

Logger::Impl::Impl(Logger::LogLevel level, const std::string& filename, int line)
:   m_stream(),
    m_level(level),
    m_line(line),
    m_basename(filename)
{ 
    formatTime();
    // collect thread ID in Linux OS and log level
    m_stream << CurrentThread::t_tid_string << " " << log_level_name[m_level] << " ";
}

void Logger::Impl::finish() {
    m_stream << " - "  << m_basename << ":" << m_line << '\n';
}

void Logger::Impl::formatTime() {
    /*
    auto timestamp = std::chrono::high_resolution_clock::now();
    std::string time_str = std::format("{:%Y %m %d %H %M %S}", timestamp);
    m_stream << time_str << " ";
    */
}

Logger::Logger(const std::string& basename, int line, Logger::LogLevel level) 
:   m_impl(level, basename, line)
{ }


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

void defaultOutputFunc(const char* msg, size_t len) {
    fwrite(msg, 1, len, stdout);
}

void defaultFlushFunc() {
    ::fflush(stdout);
}

Logger::outputFunc g_output = defaultOutputFunc;
Logger::flushFunc g_flush = defaultFlushFunc;

void Logger::setOutputFunc(Logger::outputFunc out) {
    g_output = out;
}

void Logger::setFlushFunc(Logger::flushFunc flush) {
    g_flush = flush;
}

