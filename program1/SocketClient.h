#pragma once

#include <string>

class SocketClient {
public:
    SocketClient(std::string host, int port);
    ~SocketClient();

    SocketClient(const SocketClient&) = delete;
    SocketClient& operator=(const SocketClient&) = delete;

    bool sendLine(const std::string& line);
    
private:
    bool connect();
    void close();

    std::string host_;
    int port_;
    int fd_ = -1;

    static constexpr int kTimeoutMs = 100;
};
