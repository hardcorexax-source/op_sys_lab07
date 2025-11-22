#include <iostream>
#include <string>
#include "socket_simple.cpp" // simplified single-file include (beginner-friendly)

int main(int argc, char **argv) {
    std::string addr = "127.0.0.1";
    uint16_t port = 3128;
    if (argc > 1) addr = argv[1];
    if (argc > 2) port = static_cast<uint16_t>(std::stoi(argv[2]));

    SimpleClient client(addr, port);
    if (client.connect_to_server() < 0) {
        std::cerr << "Failed to connect to " << addr << ":" << port << "\n";
        return 1;
    }
    std::cout << "Connected to server " << addr << ":" << port << "\n";

    std::string resp;
    ssize_t r = client.recv_some(resp, 256);
    if (r > 0) {
        std::cout << "Server replied: " << resp << std::endl;
    } else if (r == 0) {
        std::cout << "Server closed connection\n";
    } else {
        std::cerr << "Read error\n";
    }

    client.close_socket();
    return 0;
}
