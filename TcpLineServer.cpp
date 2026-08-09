#include "TcpLineServer.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

TcpLineServer::TcpLineServer(int port) : port_(port) {}

TcpLineServer::~TcpLineServer() {
    stop();
    closeListen();
}

bool TcpLineServer::start() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) return false;

    int yes = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port_));

    if (bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0 ||
        listen(listen_fd_, 1) < 0) {
        closeListen();
        return false;
    }

    running_ = true;
    return true;
}

void TcpLineServer::run(std::function<void(const std::string&)> onLine) {
    while (running_) {
        sockaddr_storage client_addr{};
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(listen_fd_, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            if (running_) {
                std::cerr << "[Program2] Ошибка accept: " << strerror(errno) << std::endl;
            }
            break;
        }

        int yes = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
        setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));

        std::cout << "[Program2] Клиент подключен." << std::endl;
        handleClient(client_fd, onLine);
        close(client_fd);
        std::cout << "[Program2] Клиент отключен. Ожидание..." << std::endl;
    }
}

void TcpLineServer::stop() {
    running_ = false;
    if (listen_fd_ != -1) {
        shutdown(listen_fd_, SHUT_RDWR);
    }
}

void TcpLineServer::closeListen() {
    if (listen_fd_ != -1) {
        close(listen_fd_);
        listen_fd_ = -1;
    }
}

void TcpLineServer::handleClient(int fd, const std::function<void(const std::string&)>& onLine) {
    std::string buffer;
    char chunk[1024];

    while (running_) {
        ssize_t received = recv(fd, chunk, sizeof(chunk), 0);

        if (received == 0) {
            if (!buffer.empty()) onLine(buffer);
            return;
        }

        if (received < 0) {
            if (errno == EINTR) continue;
            std::cerr << "[Program2] Ошибка чтения: " << strerror(errno) << std::endl;
            return;
        }

        buffer.append(chunk, static_cast<size_t>(received));

        size_t pos;
        while ((pos = buffer.find('\n')) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            onLine(line);
            buffer.erase(0, pos + 1);
        }
    }
}