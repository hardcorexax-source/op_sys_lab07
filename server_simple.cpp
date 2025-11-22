#include <iostream>
#include <string>
#include "socket_simple.cpp" // simplified single-file include (beginner-friendly)

int main() {
    const uint16_t port = 3128;
    SimpleServer server(port, 5);
    if (server.bind_and_listen() < 0) {
        std::cerr << "Failed to bind/listen on port " << port << "\n";
        return 1;
    }
    std::cout << "Server listening on port " << port << "\n";

    while (true) {
        struct sockaddr_in c_addr;
        int cfd = server.accept_client(c_addr);
        if (cfd < 0) continue; // accept prints error

        // Отримати IP клієнта у текстовому вигляді
        std::string ip = addr_to_str(c_addr);
        std::string out = ip + "\n";
        write(cfd, out.c_str(), out.size());

        ::close(cfd);
        std::cout << "Served client: " << ip << "\n";
    }

    server.close_socket();
    return 0;
}
