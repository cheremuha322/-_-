#pragma once

#include <functional>
#include <string>

class TcpLineServer {
public:
    explicit TcpLineServer(int port);
    ~TcpLineServer();

    TcpLineServer(const TcpLineServer&) = delete;
    TcpLineServer& operator=(const TcpLineServer&) = delete;

    bool start();
    void run(std::function<void(const std::string&)> onLine);
    void stop();

private:
    void handleClient(int fd, const std::function<void(const std::string&)>& onLine);
    void closeListen();

    int port_;
    int listen_fd_ = -1;
    bool running_ = false;
};