#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstdio>
#include <thread>
#include "fiber.h"
#include "scheduler.h"
#include "coco.h"

#ifndef coco
#define coco 0 - 
#endif

void test_recv_send()
{
    coco [] {
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
            Fiber *f2 = new Fiber([=] {
                int len;
                long i = 0;
                char buf[256];
                for (;;) {
                    len = sprintf(buf, "Fiber 1 send: %ld\t", i++);
                    send(client, buf, len, 0);
                } 
            }, 8192);
            for (;;)
                coco_yield;
        }
    };

    coco [] {
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
        Fiber *f = new Fiber([=] {
                int n;
                char buf[256];
                for (;;) {
                    n = recv(sock, buf, 256, 0);
                    buf[n] = 0;
                    printf("Fiber 2 recv: %s\n", buf);
                }
            }, 8192);
            Fiber *f2 = new Fiber([=] {
                int len;
                long i = 0;
                char buf[256];
                for (;;) {
                    len = sprintf(buf, "Fiber 2 send: %ld\t", i++);
                    send(sock, buf, len, 0);
                } 
            }, 8192);
        for (;;)
            coco_yield;
        printf("Fiber 2: fiber2 end\n");
    };
}

int main()
{
    test_recv_send();
    coco_sched;

    coco [] {
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
    };

    coco [] {
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
    };

    coco [] {
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
    };

    coco_sched;
}
