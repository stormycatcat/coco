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
    events |= EPOLLET;
    // if (fev->events == events) {
    //     fev->fiber = fiber;
    //     return;
    // }
    if (fev->fiber) {
        op = EPOLL_CTL_MOD;
    }

    fev->fiber = fiber;
    fev->events = events;
    ev.data.fd = fd;
    ev.events = events;

    if (epoll_ctl(epfd_, op, fd, &ev) != 0) {
        abort();
    }
}

void Dispatcher::remove_event(int fd)
{
    FiberEvent *fev;

    if (fd < 0 || fd > max_fd_) {
        return;
    }
    fev = &events_[fd];
    if (fev->fiber) {
        epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
        fev->fiber = nullptr;
        fev->events = 0;
    }
}

void Dispatcher::run()
{
    int nfds, fd, i;
    Fiber *fiber;
    epoll_event evs[128];

    for (;;) {
        if ((nfds = epoll_wait(epfd_, evs, 128, -1)) < 0) {
            if (errno == EINTR)
                continue;
            abort();
        }
    
        for (i = 0; i < nfds; ++i) {
            fd = evs[i].data.fd;
            fiber = events_[fd].fiber;
            fiber->revents_ = evs[i].events;
            fiber->status_ = FiberStatus::RUNNING;
        }
    }
}
