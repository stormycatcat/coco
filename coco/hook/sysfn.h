#ifndef _COCO_HOOK_SYSFN_H_
#define _COCO_HOOK_SYSFN_H_

#include <netinet/in.h>

using socket_fn_t = int (*)(int, int, int);
using accept_fn_t = int (*)(int, sockaddr *, socklen_t *);
using connect_fn_t = int (*)(int, const sockaddr *, socklen_t);
using read_fn_t = ssize_t (*)(int, void *, size_t);
using write_fn_t = ssize_t (*)(int, const void *, size_t);
using recv_fn_t = ssize_t (*)(int, void *, size_t, int);
using send_fn_t = ssize_t (*)(int, const void *, size_t, int);
using close_fn_t = int (*)(int);

extern socket_fn_t socket_fn;
extern accept_fn_t accept_fn;
extern connect_fn_t connect_fn;
extern read_fn_t read_fn;
extern write_fn_t write_fn;
extern recv_fn_t recv_fn;
extern send_fn_t send_fn;
extern close_fn_t close_fn;

#endif
