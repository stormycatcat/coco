#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <thread>
#include <iostream>
#include "coco.h"

using namespace std;

#ifndef coco
#define coco 0 - 
#endif

int main(int argc, char **argv)
{
    int opt;
    bool server = false;

    while ((opt = getopt(argc, argv, "s")) != -1) {
        switch (opt)
        {
        case 's':
            server = true;
            break;
        
        default:
            break;
        }
    }

    if (server) {
        coco [] {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in saddr;
            saddr.sin_port = htons(8888);
            saddr.sin_addr.s_addr = INADDR_ANY;
            saddr.sin_family = AF_INET;
            bind(sock, (sockaddr *)&saddr, sizeof(saddr));
            listen(sock, 5);
            for (;;) {
                int client = accept(sock, nullptr, nullptr);
                printf("accept: %d\n", client);
                coco [=] {
                    int n;
                    char buf[256];
                    for (;;) {
                        if ((n = recv(client, buf, 256, 0)) <= 0)
                            break;
                        if (send(client, buf, n, 0) <= 0)
                            break;
                        buf[n] = 0;
                        printf("%s\n", buf);
                    }
                    printf("disconnected\n");
                };
            }
        };
    } else {
        coco [] {
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            sockaddr_in saddr;
            saddr.sin_port = htons(8888);
            inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
            saddr.sin_family = AF_INET;
            if (connect(sock, (sockaddr *)&saddr, sizeof(saddr)) != 0) {
                exit(-1);
            }
            char send_buf[256];
            char recv_buf[256];
            int len;
            long i = 0;
            for (;;) {
                cin >> send_buf;
                if (send(sock, send_buf, strlen(send_buf), 0) <= 0)
                    exit(-1);
                if ((len = recv(sock, recv_buf, 256, 0)) <= 0)
                    exit(-1);
                recv_buf[len] = 0;
                printf("%s\n", recv_buf);
            }
            printf("Fiber 2: fiber2 end\n");
        };
    }

    coco_sched;
}
