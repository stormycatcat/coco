#include <netinet/in.h>
#include "hook/sysfn.h"
#include "hook/fdhook.h"
#include "scheduler.h"

extern "C" int accept(int fd, sockaddr *sa, socklen_t *len)
{
    int ret;
    FDHook *fdhook = coco_fdhook();
    FDContext *fdctx = fdhook->get_fdctx(fd);
    
    if (is_main_fiber() || !fdctx || !fdctx->is_hooked || fdctx->is_nonblock)
        return accept_fn(fd, sa, len);
    coco_scheduler()->poll_read_event(fd);
    ret = accept_fn(fd, sa, len);
    fdhook->hook(ret, false);
    return ret;
}
