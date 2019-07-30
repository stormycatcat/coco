#ifndef _COCO_DISPATCHER_H_
#define _COCO_DISPATHCER_H_

class Fiber;

struct FiberEvent
{
    int events;
    Fiber *fiber;
};

class Dispatcher
{
public:
    Dispatcher(int max_fd);

    void add_event(int fd, int events);

    void remove_event(int fd);

    void run();

private:
    int epfd_;
    int max_fd_;
    FiberEvent *events_;
};

#endif
