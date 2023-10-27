#include <unistd.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include "Buffer.h"
#include "base/Logging.h"


Buffer::Buffer()
:   m_read_index(0),
    m_write_index(0),
    m_buffer(INITTIAL_SIZE, 0)
{ }

Buffer::~Buffer() { }

void Buffer::clearUselessData() {
    std::copy(m_buffer.begin() + m_read_index, m_buffer.begin() + m_write_index, m_buffer.begin());
    m_write_index -= m_read_index;
    m_read_index = 0;
}

void Buffer::reserve(std::size_t cap) {
    m_buffer.reserve(cap);
    clearUselessData();
}

void Buffer::ensureEnoughSpace(int len) {
    if (writableBytes() < len) {
        makeSpace(len);
    }
}

void Buffer::makeSpace(int len) {
    if (preWritableBytes() + writableBytes() >= len) {
        // move msg in the buffer to the head
        int readable_bytes = readableBytes();
        std::copy(m_buffer.begin() + m_read_index,
                    m_buffer.begin() + m_write_index,
                    m_buffer.begin());
        m_read_index = 0;
        m_write_index = readable_bytes;
    }
    else {
        // or allocate more space for the buffer
        m_buffer.resize(m_write_index + len);
    }
}

void Buffer::append(const char* msg, int len) {
    // make sure the buffer has enought space to append the string 
    ensureEnoughSpace(len);
    // append the msg to the end of the buffer
    std::copy(msg, msg + len, writeBegin());
    m_write_index += len;
}

void Buffer::append(const std::string& msg) {
    append(msg.data(), msg.size());
}

std::string Buffer::getAnHTTPLine() {
    for (std::size_t i = m_read_index; i < m_write_index; ++i) {
        // an http line is end up with "/r/n"
        if (m_buffer[i] == '\r' && (i + 1 < m_write_index) && m_buffer[i + 1] == '\n') {
            int start = m_read_index;
            m_read_index = i + 2;
            return std::string(m_buffer.begin() + start, m_buffer.begin() + m_read_index);
        }
        
        if (i == m_write_index - 1) {
            return std::string(m_buffer.begin() + m_read_index, m_buffer.begin() + i);
        }
    }
    return ""; 
}

void Buffer::setContent(const std::string& request) {
    for (std::size_t i = 0; i < request.size(); ++i) {
        m_buffer[m_write_index] = request[i];
        ++m_write_index;
    }
}

ssize_t Buffer::readFromFd(int fd) {
    ssize_t all_bytes = 0;
    while (true) {
        char tmp[1024];
        ssize_t bytes = ::read(fd, tmp, 1024);
        if (bytes < 0) {
            if (errno == EINTR) continue;
            else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_ERROR << "read() error!";
                all_bytes = bytes;
            }
            break;
        }
        else if (bytes == 0) {
            // the peer close the connection
            all_bytes = 0;
            break;
        }
        all_bytes += bytes;
        append(tmp, bytes);
    }
    return all_bytes;
}

ssize_t Buffer::writeToFd(int fd) {
    ssize_t all_bytes = 0;
    while (true) {
        ssize_t bytes = ::write(fd, readBegin(), readableBytes());
        if (bytes < 0) {
            // write to socket buffer until socket buffer is full
            if (errno == EINTR) continue;
            else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                LOG_ERROR << "write() error!";
                all_bytes = bytes;
            }
            break;
        }
        else if (bytes > 0) {
            m_read_index += bytes;
            all_bytes += bytes;
            if (m_read_index == m_write_index) {
                m_read_index = 0;
                m_write_index = 0;
            }
        }
        else if (bytes == 0) {
            // the peer client close the connection
            all_bytes = 0;
            break;
        }
    }
    return all_bytes;
}

void Buffer::retrieveAll() {
    m_read_index = 0;
    m_write_index = 0;
}