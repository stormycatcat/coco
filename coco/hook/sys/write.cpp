#include "hook/sysfn.h"
#include "hook/fdhook.h"
#include "scheduler.h"

extern "C" ssize_t write(int fd, const void *buf, size_t bytes)
{
    FDContext *fdctx = coco_fdhook()->get_fdctx(fd);
    Scheduler *sched = coco_scheduler();
    ssize_t ret;
    
    if (is_main_fiber() || !fdctx || !fdctx->is_hooked || fdctx->is_nonblock)
        return write_fn(fd, buf, bytes);
    sched->add_write_event(fd);
    ret = write_fn(fd, buf, bytes);
    sched->add_write_event(fd);
    return ret;
}
