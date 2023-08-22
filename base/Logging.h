#ifndef LOGGING_H
#define LOGGING_H

#include <string>

#include "LogStream.h"


/*
format of a log line:
date + time + nanosecond + threadID + log level + msg + "-" + filename + line
Logger::Impl:   date + time + nanosecond + threadID + log level
Logger::        msg
Logger::Impl:   "-" + filename + ":" + line
*/

/*
log level:
INFO: An event happened, the event is purely informative and can be ignored during normal operations.
WARN: Unexpected behavior happened inside the progress, but it continues to work and the key business features are operating as expected.
ERROR: One or more functionalities are not working, preventing some functionalities from working correctly.
FATAL: One or more key business functionalities are not working and the whole system doesn't fulfill the business functionalities.
*/

class Logger {
    public:
        enum LogLevel { INFO, WARN, ERROR, FATAL };

        Logger(const std::string& basename, int line, LogLevel level);
        ~Logger();

        static LogLevel logLevel();

        LogStream& stream() { return m_impl.m_stream; }

        typedef void (*outputFunc)(const char* msg, size_t len);
        typedef void (*flushFunc)();
        static void setOutputFunc(outputFunc);
        static void setFlushFunc(flushFunc);
    private:
        // unaccessible for users in practical usage
        class Impl {
            public:
                typedef Logger::LogLevel LogLevel;

                Impl(LogLevel level, const std::string& basename, int line);
                void finish();
                void formatTime();

                LogStream m_stream;
                LogLevel m_level;
                int m_line;
                const std::string m_basename;                    
        }; // class Impl

        Impl m_impl;
}; // class Logger


extern Logger::LogLevel g_loglevel;

inline Logger::LogLevel Logger::logLevel() {
    return g_loglevel;
}


#define LOG_INFO if (Logger::logLevel() <= Logger::INFO) \
    Logger(__FILE__, __LINE__, Logger::LogLevel::INFO).stream()
#define LOG_WARN if (Logger::logLevel() <= Logger::WARN) \
    Logger(__FILE__, __LINE__, Logger::LogLevel::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::LogLevel::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::LogLevel::FATAL).stream()

#endif // LOGGING_H