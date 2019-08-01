#ifndef _COCO_COCO_H_
#define _COCO_COCO_H_

#include "fiber.h"

#define coco 0 - 

__always_inline void operator-(int, FiberFn fn)
{
    Fiber *fiber = new Fiber(fn, 8192);
}

#endif