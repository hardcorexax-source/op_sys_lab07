/*client.c


*/


/*server.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

void action(int) {
    wait(NULL);
}

int main() {

    signal(SIGCHLD, action);

    printf("Створення сокету\n");
    int sid = socket(AF_INET, SOCK_STREAM, 0);
    if ( sid < 0 ) {
        printf("Сокет створити не вдалося\n");
        exit(1);
    }

    int opt = 1;
    setsockopt(sid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    in_port_t port = htons(3128);

    in_addr_t addr = INADDR_ANY;

    struct sockaddr_in s_addr = { AF_INET, port, addr }; // sockaddr

    if (bind(sid, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0) {
        printf("Прив'язати не вдалося\n");
        exit(1);
    }

    if (listen(sid, 0) < 0) {
        printf("Створити чергу не вдалося\n");
        exit(1);
    }

    struct sockaddr_in c_addr;
    socklen_t s_len = sizeof(c_addr);

    while (1) {
        printf("Чекаємо запит на підключення\n");
        int cid = accept(sid, (struct sockaddr*) &c_addr, &s_len);
        printf("Підключення відбулося\n");

        if (fork() == 0) {
            /////////////
            in_addr_t user_addr = c_addr.sin_addr.s_addr;
            char buf[16] = {};
            int i = -1;
            while (user_addr) {
                i += 3;
                in_addr_t temp = user_addr % 256;
                while (temp) {
                    buf[i] = temp % 10 + '0';
                    temp /= 10;
                    --i;
                }
                user_addr /= 256;
                i += 4;
                if (user_addr) {
                    buf[i] = '.';
                }
            }
            write(cid, buf, 16);
            write(cid, "\n", 1);
            ////////

            close(cid);
            exit(0);
        }

        close(cid);
    }
}
*/

//Опис класу 
class server_socket {
  public:
    //Конструктори класу server_socket:
    server_socket(const in_port_t port) : sid(socket(AF_INET, SOCK_STREAM, 0)){ //Приймає значення порту, прослуховання по усім інтерфесам
      struct sockaddr_in s_addr = { AF_INET, htons(port), INADDR_ANY};
    }
    server_socket(const char* addr, const int_port_t port) /*Приймає IP адесу мережевого 
    інтерфейсу, який буде прослуховуватися по вказаному порту*/
    : sid(socket(AF_INET, SOCK_STREAM, 0)) {
      struct sockaddr_in s_addr = { AF_INET, htons(port), addr};
    }
    //Деструктори класу
    ~server_socket();
    //Закриває файловий дескриптор сокету
    //Методи класу:
    int sopen();
    /*Створює сокет та зі’язує його з мережевим інтерфейсом. Повертає 0, у
    разі невдачи повертає -1.*/
    client_socket saccept();
    //Прослуховує порт, чекає запит на підключення, повертае об’ект типу
    void sclose(); //Закриває файловий дескриптор сокету, кінець сеансу зв’зку.
  private:
    //Поля класу:
    pid_t sid; //Файловий дескриптор сокету
    struct sockaddr_in s_addr;
};

//Опис класу 
class client_socket {
  public:
    //Конструктори класу client_socket:
    client_socket(const pid_t sid, struct sockaddr_in s_addr);
    /*Приймає файловий дескриптор кліент сокету, та його структуру. Сокет
    вже створенний функціей saccept(), цей конструктор сокет не створює,
    він тільки приймає параметри вже існуючего сокету.*/
    client_socket(const char* addr, const int_port_t port);
    //Приймає IP адесу серверу, та порт, до якого буде здіснено підключення.
    //Деструктори класу
    ~client_socket();
    //Закриває файловий дескриптор сокету
    //Методи класу:
    int sopen();
    /*Створює сокет та робиться спроба підключитися до серверу. Повертає 0,
    у разі невдачи повертає -1.*/
    string sread(int& status);
    /*Читає данні з сокету та повертае рядок типу string. Успішність
    виконання операції повертається через параметр status (0 – Операція
    успішна, -1 у разі невдачи)*/
    int swrite(const string& s);
    //Записує рядок у сокет. Повертає 0, у разі невдачи повертає -1.
    void sclose();
    // Закриває файловий дескриптор сокету, кінець сеансу зв’зку.
    string get_addr();
    /* Повертає IP адресу сокета клієнта у вигляді точечної нотації
    (xxx.xxx.xxx.xxx). */
    in_port_t get_port();
    //Повертає port сокета.
  private:
    //Поля класу:
    pid_t sid;
    //Файловий дескриптор сокету
    struct sockaddr_in s_addr;
};