#include <sys/socket.h>
#include "hook/sysfn.h"
#include "hook/fdhook.h"

extern "C" int socket(int domain, int type, int protocol)
{
    int sockfd = socket_fn(domain, type, protocol);
    coco_fdhook()->hook(sockfd, false);
    return sockfd;
}
