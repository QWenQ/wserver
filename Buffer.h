#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>

/// +-------------------+------------------+------------------+-----------+
/// |    useless data   |  readable bytes  |  writable bytes  |           |
/// |                   |     (CONTENT)    |                  |           |
/// +-------------------+------------------+------------------+-----------|
/// |                   |                  |                  |           |
/// 0      <=      readerIndex   <=   writerIndex    <=     size   <= capacity

class Buffer {
    public:
        const static std::size_t INITTIAL_SIZE = 1024;
        Buffer();
        Buffer(int fd);
        ~Buffer();

        void read();

        void append(const std::string& msg);
        void append(const char* msg, int len);
        void write();

        std::string getAnHTTPLine();

        ssize_t readFromFd(int fd);
        ssize_t writeToFd(int fd);
        int writableBytes() const { return m_buffer.size() - m_write_index; }
        int preWritableBytes() const { return m_read_index; }
        int readableBytes() const { return m_write_index - m_read_index; }
        void ensureEnoughSpace(int len);
        void retrieveAll();
 

        // debug:
        void setContent(const std::string& request);

    private:
        // moves CONTENT back to the start of the buffer
        void clearUselessData();
        // reserves double original storage
        void reserve(std::size_t cap);

        // get the begin of the buffer
        char* begin() { return &(*m_buffer.begin()); }
        char* writeBegin() { return &(*m_buffer.begin()) + m_write_index; }
        char* readBegin() { return &(*m_buffer.begin()) + m_read_index; }
        void makeSpace(int len);


        int m_fd;
        std::size_t m_read_index;
        std::size_t m_write_index;
        std::vector<char> m_buffer;
}; // class Buffer

#endif // CHANNEL_H