#include "scheduler.h"

#include <sys/types.h>
#include <sys/epoll.h>
#include <thread>
#include "fiber.h"
#include "event/dispatcher.h"

Scheduler::Scheduler(): dispatcher_(new Dispatcher(1024))
{
    Dispatcher *d = dispatcher_;
    std::thread th([=] {
        d->run();
    });
    th.detach();
}

void Scheduler::__schedule()
{
    if (cur_fiber_id_ >= fibers_.size())
        cur_fiber_id_ = 0;
    if (fibers_[cur_fiber_id_]->status() == FiberStatus::RUNNING) {
        cur_fiber_ = fibers_[cur_fiber_id_].get();
        fibers_[cur_fiber_id_]->switch_in();
        cur_fiber_ = nullptr;
        if (fibers_[cur_fiber_id_]->status() == FiberStatus::DEAD) {
            fibers_.erase(fibers_.begin() + cur_fiber_id_);
        }
    }
    ++cur_fiber_id_;
}

void Scheduler::add_fiber(Fiber *fiber)
{
    fibers_.emplace_back(fiber);
}

void Scheduler::poll_read_event(int fd)
{
    this_fiber->status_ = FiberStatus::BLOCK;
    dispatcher_->add_event(fd, EPOLLIN);
    __yield();
}

void Scheduler::poll_write_event(int fd)
{
    this_fiber->status_ = FiberStatus::BLOCK;
    dispatcher_->add_event(fd, EPOLLOUT);
    __yield();
}

void Scheduler::poll_rdwr_event(int fd)
{
    this_fiber->status_ = FiberStatus::BLOCK;
    dispatcher_->add_event(fd, EPOLLIN | EPOLLOUT);
    __yield();
}

void Scheduler::remove_read_event(int fd)
{
    dispatcher_->remove_event(fd, EPOLLIN);
}

void Scheduler::remove_write_event(int fd)
{
    dispatcher_->remove_event(fd, EPOLLOUT);
}

void Scheduler::remove_rdwr_event(int fd)
{
    dispatcher_->remove_event(fd, EPOLLIN | EPOLLOUT);
}

void Scheduler::__yield()
{
    fibers_[cur_fiber_id_]->switch_out();
}
