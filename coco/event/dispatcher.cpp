#include "event/dispatcher.h"

#include <sys/epoll.h>
#include "fiber.h"

Dispatcher::Dispatcher(int max_fd):
    max_fd_(max_fd), events_(new FiberEvent[max_fd + 1])
{
    if ((epfd_ = epoll_create1(EPOLL_CLOEXEC)) < 0) {
        throw std::runtime_error("Dispatcher::Dispatcher : epoll_create1 error");
    }
}

void Dispatcher::add_event(int fd, int events)
{
    int op = EPOLL_CTL_ADD;
    FiberEvent *fev;
    Fiber *fiber = this_fiber;
    epoll_event ev;

    if (fd < 0 || fd > max_fd_) {
        return;
    }

    fev = &events_[fd];

    mutex_.lock();
    if (events == EPOLLIN) {
        fev->read_entries.emplace_back(this_fiber);
    } else if (events == EPOLLOUT) {
        fev->write_entries.emplace_back(this_fiber);
    } else if (events == (EPOLLIN | EPOLLOUT)) {
        fev->rdwr_entries.emplace_back(this_fiber);
    }
    mutex_.unlock();

    events |= EPOLLET;
    // if (fev->events == events) {
    //     fev->fiber = fiber;
    //     return;
    // }
    if (fev->events) {
        op = EPOLL_CTL_MOD;
    }

    fev->events |= events;
    ev.data.fd = fd;
    ev.events = fev->events;

    assert(epoll_ctl(epfd_, op, fd, &ev) == 0);
}

void Dispatcher::remove_event(int fd, int events)
{
    epoll_event ev;
    FiberEvent *fev;

    if (fd < 0 || fd > max_fd_) {
        return;
    }
    fev = &events_[fd];
    if (fev->events) {
        if (events & EPOLLIN) {
            if (fev->events & (EPOLLIN | EPOLLOUT) == EPOLLIN) {
                epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
                fev->events = 0;
            } else {
                fev->events = EPOLLOUT | EPOLLET;
                ev.data.fd = fd;
                ev.events = fev->events;
                assert(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == 0);
            }
        } else if (events & EPOLLOUT) {
            if (fev->events & (EPOLLIN | EPOLLOUT) == EPOLLOUT) {
                assert(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr) == 0);
                fev->events = 0;
            } else {
                fev->events = EPOLLIN | EPOLLET;
                ev.data.fd = fd;
                ev.events = fev->events;
                assert(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == 0);
            }
        }
    }
}

void Dispatcher::run()
{
    int nfds, fd, i;
    Fiber *fiber;
    FiberEvent *fev;
    epoll_event evs[128];

    for (;;) {
        if ((nfds = epoll_wait(epfd_, evs, 128, -1)) < 0) {
            if (errno == EINTR)
                continue;
            abort();
        }
    
        for (i = 0; i < nfds; ++i) {
            fd = evs[i].data.fd;
            fev = &events_[fd];
            if (evs[i].events & EPOLLIN) {
                std::lock_guard lock(mutex_);
                for (auto &we: fev->read_entries) {
                    fiber = we.fiber;
                    fiber->revents_ = evs[i].events;
                    fiber->status_ = FiberStatus::RUNNING;
                }
                fev->read_entries.clear();
            }
            if (evs[i].events & EPOLLOUT) {
                std::lock_guard lock(mutex_);
                for (auto &we: fev->write_entries) {
                    fiber = we.fiber;
                    fiber->revents_ = evs[i].events;
                    fiber->status_ = FiberStatus::RUNNING;
                }
                fev->write_entries.clear();
            }
            if ((evs[i].events & (EPOLLIN | EPOLLOUT)) == (EPOLLIN | EPOLLOUT)) {
                std::lock_guard lock(mutex_);
                for (auto &we: fev->rdwr_entries) {
                    fiber = we.fiber;
                    fiber->revents_ = evs[i].events;
                    fiber->status_ = FiberStatus::RUNNING;
                }
                fev->rdwr_entries.clear();
            }
        }
    }
}
