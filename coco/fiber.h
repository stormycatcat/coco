#ifndef _COCO_FIBER_H_
#define _COCO_FIBER_H_

#include <functional>
#include <cassert>
#include "fcontext.h"
#include "scheduler.h"

class FiberContext
{
public:
    FiberContext() {}

    FiberContext(std::size_t stack_size, fcontext_fn_t f, intptr_t param = 0):
        sp_(new char[stack_size]), stack_size_(stack_size), param_(param), f_(f),
        ctx_(make_fcontext(sp_ + stack_size, stack_size, f))
    {}

    void switch_to(FiberContext &other)
    {
        jump_fcontext(&ctx_, other.ctx_, param_);
    }

    void switch_from(FiberContext &other)
    {
        jump_fcontext(&other.ctx_, ctx_, param_);
    }

// private:
    char *sp_ = nullptr;
    std::size_t stack_size_ = 0;
    intptr_t param_ = 0;
    fcontext_fn_t f_ = nullptr;
    fcontext_t ctx_ = nullptr;
};

__always_inline static FiberContext &tls_main_ctx()
{
    thread_local static FiberContext fc;
    return fc;
}

using FiberFn = std::function<void()>;

enum class FiberStatus
{
    RUNNING,
    BLOCK,
    DEAD
};

class Fiber
{
public:
    template<class FiberFnType>
    Fiber(FiberFnType &&f, std::size_t stack_size):
        ctx_(stack_size, FiberFnWrapper, (intptr_t)this), f_(f)
    {
        coco_scheduler()->add_fiber(this);
    }

    FiberStatus status()
    {
        return status_;
    }

    int revents()
    {
        return revents_;
    }

// private:
    static void FiberFnWrapper(intptr_t fiber)
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
        coco_yield;
    }

// private:
    friend Scheduler;

    FiberContext ctx_ {};
    FiberFn f_ = nullptr;
    FiberStatus status_ = FiberStatus::RUNNING;
    int revents_ = 0;
};

#endif
