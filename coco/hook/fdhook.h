#ifndef _COCO_FDHOOK_H_
#define _COCO_FDHOOK_H_

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <cstdio>

struct FDContext
{
    int fd;
    uint32_t is_hooked : 1;
    uint32_t is_nonblock : 1;
    uint32_t padding : 30;

    FDContext(): is_hooked(0), is_nonblock(0)
    {}
};

class FDHook
{
public:
    FDHook(int max_fd);

    FDContext *get_fdctx(int fd)
    {
        if (fd < 0 || fd > max_fd_)
            return nullptr;
        return &fdctxs_[fd];
    }

    void hook(int fd, bool nonblock)
    {
        if (fd < 0 || fd > max_fd_)
            return;
        fdctxs_[fd].fd = fd;
        fdctxs_[fd].is_hooked = true;
        fdctxs_[fd].is_nonblock = nonblock;
    }

    void unhook(int fd)
    {
        fdctxs_[fd].is_hooked = false;
    }

private:
    int max_fd_;
    FDContext *fdctxs_;
};

__always_inline FDHook *coco_fdhook()
{
    static FDHook fdhook(0xefff);
    return &fdhook;
}

#endif
