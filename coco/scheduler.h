#ifndef _COCO_SCHEDULER_H_
#define _COCO_SCHEDULER_H_

#include <vector>
#include <cassert>

#define coco_yield  do { coco_scheduler()->__yield(); } while (0)
#define coco_sched  do {coco_scheduler()->__schedule(); } while (1)
#define this_fiber  (coco_scheduler()->__this_fiber())
        
class Fiber;
class Dispatcher;

class Scheduler
{
public:
    Scheduler();

    void __schedule();

    void add_fiber(Fiber *fiber);

    void poll_read_event(int fd);
    void poll_write_event(int fd);
    void poll_rdwr_event(int fd);
    void remove_read_event(int fd);
    void remove_write_event(int fd);
    void remove_rdwr_event(int fd);

    Fiber *__this_fiber()
    {
        return cur_fiber_;
    }

    void __yield();

private:
    Dispatcher *dispatcher_;
    std::vector<Fiber *> fibers_ {};
    Fiber *cur_fiber_ = nullptr;
    int cur_fiber_id_ = 0;
    int running_count_ = 0;
};

__always_inline Scheduler *coco_scheduler()
{
    static Scheduler scheduler;
    return &scheduler;
}

__always_inline bool is_main_fiber()
{
    return !this_fiber;
}

#endif
