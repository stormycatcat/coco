#ifndef _COCO_FIBER_H_
#define _COCO_FIBER_H_

#include <functional>
#include <cassert>
#include "context.h"
#include "scheduler.h"

using FiberFunc = std::function<void()>;

class Fiber
{
private:
    static void FiberFuncWrapper(intptr_t fiber)
    {
        assert(fiber != 0);
        Fiber *fb = (Fiber *)fiber;
        fb->start();
    }

    void start()
    {
        f_();
    }

public:
    Fiber(const FiberFunc &f, std::size_t stack_size):
        ctx_(stack_size, FiberFuncWrapper, (intptr_t)this), f_(f)
    {
        assert(ctx_.param_ != 0);
        assert((unsigned long)(&(((FiberContext *)0)->param_)) == 0x10);
    }

    void switch_in()
    {
        ctx_.switch_from(tls_main_ctx());
    }

private:
    FiberContext ctx_;
    FiberFunc f_;
};

#endif
