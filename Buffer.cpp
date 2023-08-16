#include <unistd.h>
#include <sys/uio.h>
#include "Buffer.h"


Buffer::Buffer()
:   m_read_index(0),
    m_write_index(0),
    m_buffer(INITTIAL_SIZE, 0)
{ }

Buffer::Buffer(int fd)
    :   m_fd(fd),
        m_read_index(0),
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


void Buffer::read() {
    clearUselessData();
    // writable space in m_buffer
    int writable = m_buffer.capacity() - m_write_index;
    // extra buffer
    char extrabuf[65536];
    struct iovec vec[2];
    vec[0].iov_base = m_buffer.data() + m_write_index;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);
    int veccnt = 2;
    const ssize_t nbytes = readv(m_fd, vec, veccnt);

    /*  when there is no enough memory in m_buffer, 
    **  the storage will reserved automatically.
    */
    if (nbytes < 0) {
        // todo: error handling
    }
    else if (static_cast<ssize_t>(writable) <= nbytes) {
        // no need to reserve storage
        m_write_index += nbytes;
    }
    else {
        // more memory needed than writable space in m_buffer
        reserve(m_write_index + nbytes);
        m_write_index += writable;
        int left_bytes= nbytes - writable;
        std::copy(extrabuf, extrabuf + left_bytes, m_buffer.begin() + m_write_index);
        m_write_index += left_bytes;
    }
}

void Buffer::append(const std::string& msg) {
    // todo
    std::size_t left_memory = m_buffer.capacity() - m_write_index;
    if (msg.size() > left_memory) {
        // reserve storage
        std::size_t new_cap = m_write_index + msg.size();
        reserve(new_cap);
    }
    std::copy(msg.begin(), msg.end(), m_buffer.begin() + m_write_index);
}

void Buffer::write() {
    // todo
    struct iovec vec[1];
    vec[0].iov_base = m_buffer.data() + m_read_index;
    vec[0].iov_len = m_write_index - m_read_index;
    int veccnt = 1;
    const ssize_t nbytes = ::writev(m_fd, vec, veccnt);
    if (nbytes < 0) {
        // todo: error handling
    }
    else if (static_cast<ssize_t>(m_write_index - m_read_index) > nbytes) {
        // todo: error handling ??
    }
    else {
        m_read_index += nbytes;
        clearUselessData();
    }
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
    char extrabuf[65536];
    struct iovec iovs[2];
    int writable_bytes = writableBytes();
    iovs[0].iov_base = begin() + m_write_index;
    iovs[0].iov_len = writable_bytes;
    iovs[1].iov_base = extrabuf;
    iovs[1].iov_len = sizeof(extrabuf);
    int iovcnt = 2;
    ssize_t bytes = ::readv(fd, iovs, iovcnt);
    // if m_buffer has enough space
    if (writable_bytes >= bytes) {
        m_write_index += bytes;
    }
    else {
        m_write_index = m_buffer.size();
        append(extrabuf, bytes - writable_bytes);
    }
    return bytes;
}

ssize_t Buffer::writeToFd(int fd) {
    ssize_t bytes = ::write(fd, readBegin(), readableBytes());
    if (bytes < 0) {
        perror("write() error!");
    }
    else {
        m_read_index += bytes;
        // retrive
        if (m_read_index == m_write_index) {
            m_read_index = 0;
            m_write_index = 0;
        }
    }
    return bytes;
}

void Buffer::append(const char* msg, int len) {
    // make sure the buffer has enought space to append the string 
    ensureEnoughSpace(len);
    // append the msg to the end of the buffer
    std::copy(msg, msg + len, writeBegin());
    m_write_index += len;
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

void Buffer::retrieveAll() {
    m_read_index = 0;
    m_write_index = 0;
}