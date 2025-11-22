#include <iostream>
#include "socket.cpp"

using namespace std;

int main() {
    cout << "Старт клієнтської програми" << endl;
    
    client_socket client("127.0.0.1", 3128);
    
    cout << "Підключення до сервера..." << endl;
    
    // Відкриваємо з'єднання
    if (client.sopen() < 0) {
        cerr << "Помилка: не вдалося підключитися до сервера" << endl;
        return 1;
    }
    
    cout << "Підключення успішне!" << endl;
    
    // Читаємо відповідь від сервера
    int status;
    string response = client.sread(status);
    
    if (status < 0) {
        cerr << "Помилка при читанні даних від сервера" << endl;
    } else {
        cout << "\nВідповідь від сервера:\n" << response << endl;
    }
    
    // Закриваємо з'єднання
    cout << "Завершення сесії" << endl;
    client.sclose();
    
    return 0;
}