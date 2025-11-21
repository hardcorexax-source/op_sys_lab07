#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <string.h>

int main() {
    printf("Старт програми\n");

    int cid = socket(AF_INET, SOCK_STREAM, 0);

    if (cid < 0) {
        printf("Сокет створити не вдалось\n");
        exit(1);
    }

    struct sockaddr_in c_addr = {AF_INET, htons(3128), inet_addr("146.120.160.205")};

    printf("Підключення...\n");

    int res = connect(cid, (struct sockaddr*)&c_addr, sizeof(c_addr));

    if (res < 0) {
        printf("Підключення до серверу не вдалося!\n");
        exit(1);
    }
    printf("ОК\n");

    char buf[4096] = {};
    char bufin[4096] = {};

    do {
        printf("=> ");
        scanf("%s", buf); // fgets()

        write(cid, buf, strlen(buf));

        int len = read(cid, bufin, sizeof(buf));

        printf("%s\n", bufin);
    } while (strcmp(buf, "exit") != 0);

    printf("Завершення сесії\n");
    close(cid);

    exit(0);
}