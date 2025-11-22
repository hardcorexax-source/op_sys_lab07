#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

using namespace std;

class client_socket;

class server_socket {
public:
    // Конструктор: приймає значення порту, прослуховування по всім інтерфейсам
    server_socket(const in_port_t port);
    
    // Конструктор: приймає IP адресу мережевого інтерфейсу та порт
    server_socket(const char* addr, const in_port_t port);
    
    // Деструктор
    ~server_socket();
    
    // Створює сокет та зв'язує його з мережевим інтерфейсом
    int sopen();
    
    // Прослуховує порт, чекає запит на підключення
    client_socket saccept();
    
    // Закриває файловий дескриптор сокету
    void sclose();

private:
    int sid;  // Файловий дескриптор сокету
    struct sockaddr_in s_addr;
    bool is_open;
};

class client_socket {
public:
    // Конструктор для сервера (приймає вже створений сокет)
    client_socket(const int sid, struct sockaddr_in c_addr);
    
    // Конструктор для клієнта (створює підключення до сервера)
    client_socket(const char* addr, const in_port_t port);
    
    // Деструктор
    ~client_socket();
    
    // Створює сокет та підключається до сервера
    int sopen();
    
    // Читає дані з сокету
    string sread(int& status);
    
    // Записує рядок у сокет
    int swrite(const string& s);
    
    // Закриває файловий дескриптор сокету
    void sclose();
    
    // Повертає IP адресу клієнта
    string get_addr();
    
    // Повертає порт сокета
    in_port_t get_port();

private:
    int sid;  // Файловий дескриптор сокету
    struct sockaddr_in s_addr;
    bool is_open;
    bool is_server_side;  // true якщо створено через accept()
};


// ============== SERVER_SOCKET ==============

server_socket::server_socket(const in_port_t port) 
    : sid(-1), is_open(false) {
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = INADDR_ANY;
}

server_socket::server_socket(const char* addr, const in_port_t port) 
    : sid(-1), is_open(false) {
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = inet_addr(addr);
}

server_socket::~server_socket() {
    if (is_open) {
        sclose();
    }
}

int server_socket::sopen() {
    // Створення сокету
    sid = socket(AF_INET, SOCK_STREAM, 0);
    if (sid < 0) {
        return -1;
    }
    
    // Дозволити повторне використання адреси
    int opt = 1;
    setsockopt(sid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Прив'язка до адреси
    if (bind(sid, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
        close(sid);
        return -1;
    }
    
    // Початок прослуховування
    if (listen(sid, 5) < 0) {
        close(sid);
        return -1;
    }
    
    is_open = true;
    return 0;
}

client_socket server_socket::saccept() {
    struct sockaddr_in c_addr;
    socklen_t c_len = sizeof(c_addr);
    
    int cid = accept(sid, (struct sockaddr*)&c_addr, &c_len);
    
    return client_socket(cid, c_addr);
}

void server_socket::sclose() {
    if (is_open && sid >= 0) {
        close(sid);
        is_open = false;
    }
}

// ============== CLIENT_SOCKET ==============

client_socket::client_socket(const int sid, struct sockaddr_in c_addr) 
    : sid(sid), s_addr(c_addr), is_open(sid >= 0), is_server_side(true) {
}

client_socket::client_socket(const char* addr, const in_port_t port) 
    : sid(-1), is_open(false), is_server_side(false) {
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = inet_addr(addr);
}

client_socket::~client_socket() {
    if (is_open) {
        sclose();
    }
}

int client_socket::sopen() {
    if (is_server_side) {
        return 0;  // Вже відкритий через accept()
    }
    
    // Створення сокету
    sid = socket(AF_INET, SOCK_STREAM, 0);
    if (sid < 0) {
        return -1;
    }
    
    // Підключення до сервера
    if (connect(sid, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
        close(sid);
        return -1;
    }
    
    is_open = true;
    return 0;
}

string client_socket::sread(int& status) {
    char buffer[4096] = {};
    int len = read(sid, buffer, sizeof(buffer) - 1);
    
    if (len < 0) {
        status = -1;
        return "";
    }
    
    status = 0;
    buffer[len] = '\0';
    return string(buffer);
}

int client_socket::swrite(const string& s) {
    int result = write(sid, s.c_str(), s.length());
    return (result < 0) ? -1 : 0;
}

void client_socket::sclose() {
    if (is_open && sid >= 0) {
        close(sid);
        is_open = false;
    }
}

string client_socket::get_addr() {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(s_addr.sin_addr), ip, INET_ADDRSTRLEN);
    return string(ip);
}

in_port_t client_socket::get_port() {
    return ntohs(s_addr.sin_port);
}






