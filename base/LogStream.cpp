#include "LogStream.h"
#include <string>
#include <algorithm>


const char digits[] = "0123456789ABCDEF";

// convert T type value into string and add the string to the buffer
template <typename T>
size_t convert(char* buf, T t, bool is_hex) {
    T tmp = t;
    char* p = buf;
    int mod = is_hex ? 16 : 10;
    if (is_hex) {
        *p = '0';
        ++p;
        *p = 'x';
        ++p;
    }
    // convert t to string
    do {
        int index = static_cast<int>(tmp % mod);
        tmp /= mod;
        *p = digits[index];
        ++p;
    } while (tmp != 0);

    // if t is a dicimal and negative value
    if (!is_hex && t < 0) {
        *p = '-';
        ++p;
    }
    // add a 0 as end
    *p = '\0';
    // reverse the string value
    if (is_hex) {
        std::reverse(buf + 2, p);
    }
    else {
        std::reverse(buf, p);
    }
    return p - buf;
}


template <typename T>
void LogStream::formatInteger(T t, bool is_hex) {
    // if m_buffer has enough space
    if (m_buffer.available() > kMaxNumericSize) {
        size_t len = convert(m_buffer.current(), t , is_hex);
        m_buffer.addLen(len);
    }
}


LogStream::self& LogStream::operator<<(bool v) {
    m_buffer.append(v ? "1" : "0", 1);
    return *this;
}

LogStream::self& LogStream::operator<<(short n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(unsigned short n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(int n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(unsigned int n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(long n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(unsigned long n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(long long n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(unsigned long long n) {
    formatInteger(n, false);
    return *this;
}

LogStream::self& LogStream::operator<<(const void* msg) {
    uintptr_t p = reinterpret_cast<uintptr_t>(msg);
    formatInteger(p, true);
    return *this;
}

LogStream::self& LogStream::operator<<(float n) {
    *this << static_cast<double>(n);
    return *this;
}

LogStream::self& LogStream::operator<<(double n) {
    if (m_buffer.available() > kMaxNumericSize) {
        size_t len = snprintf(m_buffer.current(), kMaxNumericSize, "%.12g", n);
        m_buffer.addLen(len);
    }
    return *this;
}

LogStream::self& LogStream::operator<<(char ch) {
    m_buffer.append(&ch, 1);
    return *this;
}

LogStream::self& LogStream::operator<<(const char* msg) {
    m_buffer.append(msg, strlen(msg));
    return *this;
}

LogStream::self& LogStream::operator<<(const unsigned char* msg) {
    *this << reinterpret_cast<const char*>(msg);
    return *this;
}

LogStream::self& LogStream::operator<<(const std::string& msg) {
    m_buffer.append(msg.c_str(), msg.size());
    return *this;
}

LogStream::self& LogStream::operator<<(const Buffer& buf) {
    m_buffer.append(buf.data(), buf.length());
    return *this;
}