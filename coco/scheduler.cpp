#include "scheduler.h"
#include "fiber.h"

void Scheduler::schedule()
{
    if (fibers_.empty())
        return;
    if (cur_fiber_id_ >= fibers_.size())
        cur_fiber_id_ = 0;
    if (fibers_[cur_fiber_id_]->status() == FiberStatus::RUNNING) {
        fibers_[cur_fiber_id_]->switch_in();
        if (fibers_[cur_fiber_id_]->status() == FiberStatus::DEAD) {
            fibers_.erase(fibers_.begin() + cur_fiber_id_);
        }
    }
    ++cur_fiber_id_;
}

void Scheduler::add_fiber(Fiber *fiber)
{
    fibers_.push_back(fiber);
}

void Scheduler::__yield()
{
    fibers_[cur_fiber_id_]->switch_out();
}
