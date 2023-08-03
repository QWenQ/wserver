#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include <string.h>

#include "noncopyable.h"

const int kSmallBufferSize = 4000;
const int kLargerBufferSize = 4000 * 1000;

template <int SIZE>
class FixedBuffer {
    public:
        FixedBuffer() 
        :   m_buf(new char[SIZE]),
            m_cur_index(0)
        { 
            ::bzero(m_buf, SIZE);
        }

        ~FixedBuffer() {
            delete[] m_buf;
            m_buf = nullptr;
        }

        void append(const char* msg, size_t len) {
            strncpy(m_buf + m_cur_index, msg, len);
            m_cur_index += len;
        }

        char* data() const {
            return m_buf;
        }

        size_t length() const {
            return m_cur_index;
        }

        size_t available() const {
            return static_cast<size_t>(SIZE - m_cur_index);
        }

        void addLen(size_t len) { m_cur_index += len; }

        char* current() const { return m_buf + m_cur_index; }


    private:
        char* m_buf;
        int m_cur_index;
}; // class FixedBuffer


class LogStream : noncopyable {
    private:
        typedef LogStream self;

    public:
        typedef FixedBuffer<kSmallBufferSize> Buffer;
        self& operator<<(bool);
        self& operator<<(short);
        self& operator<<(unsigned short);
        self& operator<<(int);
        self& operator<<(unsigned int);
        self& operator<<(long);
        self& operator<<(unsigned long);
        self& operator<<(long long);
        self& operator<<(unsigned long long);
        self& operator<<(const void*);
        self& operator<<(float);
        self& operator<<(double);
        self& operator<<(char);
        self& operator<<(const char*);
        self& operator<<(const unsigned char*);
        self& operator<<(const std::string&);
        self& operator<<(const Buffer&);


        void append(const char* msg, size_t len) { m_buffer.append(msg, len); }

        char* buffer() const { return m_buffer.data(); }
        size_t size() const { return m_buffer.length(); }

    private:
        template <typename T>
        void formatInteger(T, bool);

        Buffer m_buffer;

        static const int kMaxNumericSize = 48;
}; // class LogStream
#endif // LOG_STREAM_H