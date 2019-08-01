#ifndef _COCO_DISPATCHER_H_
#define _COCO_DISPATHCER_H_

#include <vector>
#include <mutex>

class Fiber;

struct WakeupEntry
{
    Fiber *fiber;

    WakeupEntry(Fiber *_fiber): fiber(_fiber)
    {}
};

struct FiberEvent
{
    int events;
    std::vector<WakeupEntry> read_entries;
    std::vector<WakeupEntry> write_entries;
    std::vector<WakeupEntry> rdwr_entries;
};

class Dispatcher
{
public:
    Dispatcher(int max_fd);

    void add_event(int fd, int events);

    void remove_event(int fd, int events);

    void run();

private:
    int epfd_;
    int max_fd_;
    std::mutex mutex_;
    FiberEvent *events_;
};

#endif
