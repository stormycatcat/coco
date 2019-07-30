#include <dlfcn.h>
#include <cassert>
#include "hook/sysfn.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

socket_fn_t socket_fn;
accept_fn_t accept_fn;
connect_fn_t connect_fn;
read_fn_t read_fn;
write_fn_t write_fn;
recv_fn_t recv_fn;
send_fn_t send_fn;
close_fn_t close_fn;

__attribute__((constructor))
static void hook_init()
{
    connect_fn = (connect_fn_t) dlsym(RTLD_NEXT, "connect");
    assert(connect_fn);
    socket_fn = (socket_fn_t) dlsym(RTLD_NEXT, "socket");
    accept_fn = (accept_fn_t) dlsym(RTLD_NEXT, "accept");
    write_fn = (write_fn_t) dlsym(RTLD_NEXT, "write");
    read_fn = (read_fn_t) dlsym(RTLD_NEXT, "read");
    recv_fn = (recv_fn_t) dlsym(RTLD_NEXT, "recv");
    send_fn = (send_fn_t) dlsym(RTLD_NEXT, "send");
    close_fn = (close_fn_t) dlsym(RTLD_NEXT, "close");
}
