#ifndef _COCO_SCHEDULE_H_
#define _COCO_SCHEDULE_H_

#include <vector>

#define yield   __default_scheder().__yield()
#define sched   __default_scheder().schedule()

class Fiber;

class Scheduler
{
public:
    void schedule();

    void add_fiber(Fiber *fiber);

    void __yield();

private:
    std::vector<Fiber *> fibers_ {};
    int cur_fiber_id_ = 0;
};

__always_inline Scheduler &__default_scheder()
{
    static Scheduler scheduler;
    return scheduler;
}

#endif
