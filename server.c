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
            ////////*/

            close(cid);
            exit(0);
        }

        close(cid);
    }
}