#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include "socket.cpp"

using namespace std;

void sigchld_handler(int) {
    wait(NULL);
}

int main() {
    // Встановлюємо обробник сигналу для завершених дочірніх процесів
    signal(SIGCHLD, sigchld_handler);
    
    cout << "Створення серверного сокету на порту 3128..." << endl;
    
    // Створюємо серверний сокет
    server_socket server(3128);
    
    // Відкриваємо сокет (bind + listen)
    if (server.sopen() < 0) {
        cerr << "Помилка: не вдалося відкрити серверний сокет" << endl;
        return 1;
    }
    
    cout << "Сервер запущено. Очікування підключень..." << endl;
    
    while (true) {
        cout << "\nЧекаємо запит на підключення..." << endl;
        
        // Приймаємо підключення клієнта
        client_socket client = server.saccept();
        
        cout << "Підключення відбулося!" << endl;
        cout << "IP клієнта: " << client.get_addr() << endl;
        cout << "Порт клієнта: " << client.get_port() << endl;
        
        // Створюємо дочірній процес для обробки клієнта
        if (fork() == 0) {
            // Дочірній процес
            
            // Отримуємо IP адресу клієнта
            string client_ip = client.get_addr();
            
            // Формуємо відповідь
            string response = "Ваша IP адреса: " + client_ip + "\n";
            
            // Відправляємо відповідь клієнту
            if (client.swrite(response) < 0) {
                cerr << "Помилка при відправці даних клієнту" << endl;
            } else {
                cout << "Відправлено клієнту: " << response;
            }
            
            // Закриваємо з'єднання з клієнтом
            client.sclose();
            
            // Завершуємо дочірній процес
            exit(0);
        }
        
        // Батьківський процес закриває свою копію клієнтського сокету
        client.sclose();
    }
    
    return 0;
}