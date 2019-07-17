#ifndef _COCO_FIBER_H_
#define _COCO_FIBER_H_

#include <functional>
#include <cassert>
#include "context.h"
#include "scheduler.h"

using FiberFunc = std::function<void()>;

enum class FiberStatus
{
    RUNNING,
    DEAD
};

class Fiber
{
public:
    Fiber(const FiberFunc &f, std::size_t stack_size):
        ctx_(stack_size, FiberFuncWrapper, (intptr_t)this), f_(f)
    {
        __default_scheder().add_fiber(this);
    }

    FiberStatus status()
    {
        return status_;
    }

private:
    static void FiberFuncWrapper(intptr_t fiber)
    {
        assert(fiber != 0);
        Fiber *fb = (Fiber *)fiber;
        fb->start();
    }

    void switch_in()
    {
        ctx_.switch_from(tls_main_ctx());
    }

    void switch_out()
    {
        ctx_.switch_to(tls_main_ctx());
    }

    void start()
    {
        f_();
        f_ = nullptr;
        status_ = FiberStatus::DEAD;
        yield;
    }

private:
    friend Scheduler;
    FiberContext ctx_;
    FiberFunc f_;
    FiberStatus status_ = FiberStatus::RUNNING;
};

#endif
