#include "SocketClient.h"

#include <arpa/inet.h>
#include <cerrno>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

SocketClient::SocketClient(std::string host, int port)
    : host_(std::move(host)), port_(port) {}

SocketClient::~SocketClient() { close(); }

bool SocketClient::sendLine(const std::string& line) {
    std::string payload = line + "\n";

    for (int i = 0; i < 2; ++i) {
        if (!connect()) return false;

        size_t sent = 0;
        bool ok = true;
        while (sent < payload.size()) {
            ssize_t n = ::send(fd_, payload.data() + sent, payload.size() - sent, MSG_NOSIGNAL);
            if (n <= 0) {
                if (n < 0 && errno == EINTR) continue;
                ok = false;
                break;
            }
            sent += static_cast<size_t>(n);
        }
        if (ok) return true;
        close();
    }
    return false;
}

bool SocketClient::connect() {
    if (fd_ >= 0) return true;

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* res = nullptr;
    if (getaddrinfo(host_.c_str(), std::to_string(port_).c_str(), &hints, &res) != 0)
        return false;

    for (addrinfo* ai = res; ai; ai = ai->ai_next) {
        fd_ = ::socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd_ < 0) continue;

        int flags = fcntl(fd_, F_GETFL, 0);
        fcntl(fd_, F_SETFL, flags | O_NONBLOCK);

        int rc = ::connect(fd_, ai->ai_addr, ai->ai_addrlen);
        bool connected = (rc == 0);

        if (!connected && (errno == EINPROGRESS || errno == EWOULDBLOCK)) {
            pollfd pfd{fd_, POLLOUT, 0};
            if (poll(&pfd, 1, kTimeoutMs) > 0) {
                int err = 0;
                socklen_t len = sizeof(err);
                if (getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &len) == 0 && err == 0)
                    connected = true;
            }
        }

        if (connected) {
            fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
            timeval tv{0, kTimeoutMs * 1000};
            setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
            int yes = 1;
            setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
            freeaddrinfo(res);
            return true;
        }
        close();
    }

    freeaddrinfo(res);
    return false;
}

void SocketClient::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}