#include "hook/sysfn.h"
#include "hook/fdhook.h"
#include "scheduler.h"

extern "C" ssize_t send(int fd, const void *buf, size_t bytes, int flags)
{
    FDContext *fdctx = coco_fdhook()->get_fdctx(fd);
    Scheduler *sched = coco_scheduler();
    ssize_t ret;
    
    if (is_main_fiber() || !fdctx || !fdctx->is_hooked || fdctx->is_nonblock)
        return send_fn(fd, buf, bytes, flags);
    sched->poll_write_event(fd);
    ret = send_fn(fd, buf, bytes, flags);
    sched->poll_write_event(fd);
    return ret;
}
