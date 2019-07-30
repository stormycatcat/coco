#ifndef _HOOK_UTILITY_H_
#define _HOOK_UTILITY_H_

#include <fcntl.h>
#include "hook/fdhook.h"

class NonblockGuard
{
public:
    NonblockGuard(FDContext *fdctx): fdctx_(fdctx)
    {
        flags_ = fcntl(fdctx->fd, F_GETFD);
        if (flags_ & O_NONBLOCK) {
            return;
        }
        fcntl(fdctx->fd, F_SETFD, flags_ | O_NONBLOCK);
        fdctx->is_nonblock = true;
    }

    ~NonblockGuard()
    {
        if (flags_ & O_NONBLOCK)
            return;
        fcntl(fdctx_->fd, F_SETFD, flags_);
        fdctx_->is_nonblock = false;
    }

private:
    FDContext *fdctx_;
    int flags_;
};

#endif
