// Простий набір класів для роботи з TCP-сокетами
// Без окремих .h файлів — все в одному .cpp для наочності

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <iostream>

// Very small helper: convert sockaddr_in -> dotted IP string
static std::string addr_to_str(const struct sockaddr_in &a) {
    char buf[INET_ADDRSTRLEN] = {0};
    inet_ntop(AF_INET, &a.sin_addr, buf, sizeof(buf));
    return std::string(buf);
}

// Простий клієнт
class SimpleClient {
public:
    SimpleClient(const std::string &addr, uint16_t port)
        : fd_(-1) {
        std::memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        inet_pton(AF_INET, addr.c_str(), &addr_.sin_addr);
    }

    // Підключитись до сервера. Повертає 0 — успіх, -1 — помилка
    int connect_to_server() {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) {
            perror("socket");
            return -1;
        }
        if (connect(fd_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
            perror("connect");
            ::close(fd_);
            fd_ = -1;
            return -1;
        }
        return 0;
    }

    // Прочитати до max_len байт в рядок
    ssize_t recv_some(std::string &out, size_t max_len = 4096) {
        if (fd_ < 0) return -1;
        out.resize(max_len);
        ssize_t r = read(fd_, &out[0], max_len);
        if (r > 0) out.resize(r);
        else out.clear();
        return r;
    }

    // Надіслати рядок
    ssize_t send_all(const std::string &s) {
        if (fd_ < 0) return -1;
        return write(fd_, s.c_str(), s.size());
    }

    void close_socket() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    std::string peer_addr() const { return addr_to_str(addr_); }
    uint16_t peer_port() const { return ntohs(addr_.sin_port); }

private:
    int fd_;
    struct sockaddr_in addr_;
};

// Простий сервер, який вміє bind/listen і accept
class SimpleServer {
public:
    SimpleServer(uint16_t port, int backlog = 5)
        : fd_(-1), backlog_(backlog) {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ < 0) {
            perror("socket");
            return;
        }
        int opt = 1;
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        std::memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = INADDR_ANY;
        addr_.sin_port = htons(port);
    }

    int bind_and_listen() {
        if (fd_ < 0) return -1;
        if (bind(fd_, (struct sockaddr*)&addr_, sizeof(addr_)) < 0) {
            perror("bind");
            return -1;
        }
        if (listen(fd_, backlog_) < 0) {
            perror("listen");
            return -1;
        }
        return 0;
    }

    // Прийняти з'єднання. Повертає файловий дескриптор клієнта або -1
    int accept_client(struct sockaddr_in &client_addr) {
        socklen_t len = sizeof(client_addr);
        int cfd = accept(fd_, (struct sockaddr*)&client_addr, &len);
        if (cfd < 0) {
            perror("accept");
            return -1;
        }
        return cfd;
    }

    void close_socket() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

private:
    int fd_;
    struct sockaddr_in addr_;
    int backlog_;
};
