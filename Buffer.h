#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>

/// +-------------------+------------------+------------------+
/// |    useless data   |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size

class Buffer {
    public:
        const static std::size_t INITTIAL_SIZE = 1024;
        Buffer(int fd);
        ~Buffer();

        void read();

        void append(const std::string& msg);
        void write();

        std::string getAnHTTPLine();

        // debug:
        void setContent(const std::string& request);

    private:
        // moves CONTENT back to the start of the buffer
        void clearUselessData();
        // reserves double original storage
        void reserve(std::size_t cap);


        int m_fd;
        std::size_t m_read_index;
        std::size_t m_write_index;
        std::vector<char> m_buffer;
}; // class Buffer

#endif // CHANNEL_H