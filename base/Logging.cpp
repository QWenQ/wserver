#include "Logging.h"
#include "CurrentThread.h"
#include "timeUtils.h"
#include "AsyncLog.h"

#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>


const char* log_level_name[]{ "DEBUG", "INFO", "ERROR", "FATAL" };

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


const off_t kRollSize = 500 * 1000 * 1000;
std::unique_ptr<AsyncLog> g_asylog;
static pthread_once_t once_control = PTHREAD_ONCE_INIT;


void init_routine() {
    g_asylog.reset(new AsyncLog(::basename(__FILE__), kRollSize));
    g_asylog->start();
}

Logger::Logger(const std::string& basename, int line, Logger::LogLevel level) 
:   m_impl(level, basename, line)
{ }


void defaultOutputFunc(const char* msg, size_t len) {
    int ret = pthread_once(&once_control, init_routine);
    if (ret != 0) {
        perror("pthread_once() failed:");
        abort();
    }
    g_asylog->append(msg, len);
}

void defaultFlushFunc() {
    // ::fflush(stdout);
    // notify g_asylog to flush its all buffered data into log file
    g_asylog->flush();
}

Logger::outputFunc g_output = defaultOutputFunc;
Logger::flushFunc g_flush = defaultFlushFunc;

Logger::~Logger() {
    // before destruction of Logger object,
    // write data left in the buffer of m_impl.m_stream to the asyncLog thread
    m_impl.finish();
    g_output(stream().buffer(), stream().size());
    if (m_impl.m_level == Logger::FATAL) {
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

