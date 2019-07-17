
#include <cstdlib>
#include <cstdio>
#include "context.h"
#include "fiber.h"
#include "scheduler.h"

void fn1(intptr_t);
void fn2(intptr_t);

FiberContext fb1(4096, fn1, 1);
FiberContext fb2(4096, fn2, 1);

void fn1(intptr_t vp)
{
    printf("fn1 begin\n");
    printf("fn1 yield\n");
    fb1.switch_to(fb2);
    printf("fn1 return\n");
    fb1.switch_to(tls_main_ctx());
}

void fn2(intptr_t)
{
    printf("fn2 begin\n");
    fb2.switch_to(fb1);
}

int main()
{
    // tls_main_ctx().switch_to(fb1);
    // printf("main return\n");

    Fiber fiber1([]{
        for (;;) {
            printf("haha\n");
            yield;
        }
    }, 4096);

    Fiber fiber2([]{
        for (int i = 0; i < 1000; ++i) {
            printf("aaaaaaaaaaa\n");
            yield;
        }
    }, 4096);

    for (;;) {
        sched;
    }
}
