#ifndef _COCO_CONTEXT_H_
#define _COCO_CONTEXT_H_

#include "fcontext.h"

using FCtxFunc = void (*)(intptr_t);

class FiberContext
{
public:
    FiberContext() {}

    FiberContext(std::size_t stack_size, FCtxFunc f, intptr_t param = 0):
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
    FCtxFunc f_ = nullptr;
    fcontext_t ctx_ = nullptr;
};

__always_inline FiberContext &tls_main_ctx()
{
    thread_local static FiberContext fc;
    return fc;
}

#endif
