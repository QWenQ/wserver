#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

const char* ip = "127.0.0.1";
const int port = 1234;

char buf[4096];

void setNonBlockFd(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

int main(int argc, char* argv[]) {
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    // setNonBlockFd(sockfd);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);
    int ret = ::connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == 0) {
        std::string request = "GET / HTTP/1.0\r\n\r\n";
        write(sockfd, request.c_str(), request.size());
        int cnt = read(sockfd, buf, sizeof(buf) - 1);
        if (cnt < 0) {
            perror("read() error");
        }
        std::cout << buf << std::endl;
    }
    else {
        std::cout << errno << std::endl;
        perror("connect() error");
    }
    close(sockfd);
    return 0;
}
