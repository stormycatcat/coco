#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include "fiber.h"
#include "scheduler.h"

int main()
{
    Fiber fiber1([]{
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in saddr;
        saddr.sin_port = htons(8888);
        saddr.sin_addr.s_addr = INADDR_ANY;
        saddr.sin_family = AF_INET;
        bind(sock, (sockaddr *)&saddr, sizeof(saddr));
        listen(sock, 5);
        printf("Fiber 1: listen\n");
        for (;;) {
            int client = accept(sock, nullptr, nullptr);
            printf("Fiber 1: accept success: fd: %d\n", client);
            Fiber *f = new Fiber([=] {
                int n;
                char buf[256];
                for (;;) {
                    n = recv(client, buf, 256, 0);
                    buf[n] = 0;
                    printf("Fiber 1 recv: %s\n", buf);
                }
            }, 8192);
        }
    }, 8192);

    Fiber fiber2([]{
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in saddr;
        saddr.sin_port = htons(8888);
        inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
        saddr.sin_family = AF_INET;
        if (connect(sock, (sockaddr *)&saddr, sizeof(saddr)) != 0) {
            printf("Fiber 2: connect error\n");
            return;
        }
        printf("Fiber 2: successfully connect\n");
        char buf[256];
        int len;
        long i = 0;
        for (;;) {
            len = sprintf(buf, "Fiber 2: %ld\t", i++);
            send(sock, buf, len, 0);
        }
        printf("Fiber 2: fiber2 end\n");
    }, 8192);

    Fiber fiber3([]{
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in saddr;
        saddr.sin_port = htons(8888);
        inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
        saddr.sin_family = AF_INET;
        if (connect(sock, (sockaddr *)&saddr, sizeof(saddr)) != 0) {
            printf("Fiber 3: connect error\n");
            return;
        }
        printf("Fiber 3: successfully connect\n");
        char buf[256];
        int len;
        long i = 0;
        for (;;) {
            len = sprintf(buf, "Fiber 3: %ld\t", i--);
            send(sock, buf, len, 0);
        }
        printf("Fiber 3: fiber3 end\n");
    }, 8192);

    coco_sched;
}
