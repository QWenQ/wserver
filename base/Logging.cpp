#include "Logging.h"
#include "CurrentThread.h"

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
    m_stream << CurrentThread::tidString() << " " << log_level_name[m_level] << " ";
}

void Logger::Impl::finish() {
    m_stream << " - "  << m_basename << ":" << m_line << '\n';
}

void Logger::Impl::formatTime() {
    using namespace std::chrono;
    using clock = high_resolution_clock;
    
    const auto current_time_point {clock::now()};
    const auto current_time {clock::to_time_t (current_time_point)};
    const auto current_localtime {*std::localtime (&current_time)};
    const auto current_time_since_epoch {current_time_point.time_since_epoch()};
    const auto current_milliseconds {duration_cast<milliseconds> (current_time_since_epoch).count() % 1000};
    
    std::ostringstream stream;
    stream << std::put_time (&current_localtime, "%F %T") << ":" << std::setw (3) << std::setfill ('0') << current_milliseconds;
    
    m_stream << stream.str() << " ";
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

