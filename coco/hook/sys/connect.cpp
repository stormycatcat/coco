#include <sys/epoll.h>
#include "hook/sysfn.h"
#include "hook/fdhook.h"
#include "hook/utility.h"
#include "fiber.h"
#include "scheduler.h"

extern "C" int connect(int fd, const sockaddr *sa, socklen_t len)
{
    FDContext *fdctx = coco_fdhook()->get_fdctx(fd);
    Scheduler *sched = coco_scheduler();

    if (is_main_fiber() || !fdctx || !fdctx->is_hooked || !fdctx->is_nonblock)
        return connect_fn(fd, sa, len);
    {
        NonblockGuard guard(fdctx);
        if (connect_fn(fd, sa, len) == -1) {
            if (errno != EINPROGRESS)
                return -1;
            sched->poll_write_event(fd);
            sched->remove_write_event(fd);
            if (this_fiber->revents() & (EPOLLERR | EPOLLHUP)) {
                errno = ECONNREFUSED;
                return -1;
            }
            return 0;
        }
        return 0;
    }
}
