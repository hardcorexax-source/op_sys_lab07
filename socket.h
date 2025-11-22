// Lightweight TCP socket wrapper - header
#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <cstdint>
#include <netinet/in.h>

class TcpClient {
public:
    TcpClient(const std::string &addr, uint16_t port);
    TcpClient(int fd, struct sockaddr_in addr);
    ~TcpClient();

    // Connect to the server (only for the addr/port constructor)
    // Throws std::runtime_error on error
    void connect_to_server();

    // Send data (returns number of bytes sent, -1 on error)
    ssize_t send_all(const std::string &data);

    // Receive up to max_len bytes into out (returns number of bytes received, 0 = peer closed, -1 on error)
    ssize_t recv_some(std::string &out, size_t max_len = 4096);

    // Close socket
    void close_socket();

    std::string peer_addr() const;
    uint16_t peer_port() const;
    bool is_open() const;

private:
    int fd_ = -1;
    struct sockaddr_in addr_ = {};
    bool owns_fd_ = true;
};

class TcpServer {
public:
    TcpServer(uint16_t port, int backlog = 5);
    ~TcpServer();

    // Bind and start listening. Throws std::runtime_error on error.
    void bind_and_listen();

    // Accept a client. Throws std::runtime_error on error.
    TcpClient accept_client();

    void close_socket();
    bool is_open() const;

private:
    int fd_ = -1;
    struct sockaddr_in addr_ = {};
    int backlog_ = 5;
};

#endif // SOCKET_H
