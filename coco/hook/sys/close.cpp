#include "hook/sysfn.h"
#include "hook/fdhook.h"
#include "scheduler.h"

extern "C" int close(int fd)
{
    int ret;
    FDHook *fdhook = coco_fdhook();
    FDContext *fdctx = fdhook->get_fdctx(fd);

    if (fdctx)
        fdctx->is_hooked = false;

    return close_fn(fd);
}
