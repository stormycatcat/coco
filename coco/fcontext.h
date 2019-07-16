//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef _COCO_FCONTEXT_H_
#define _COCO_FCONTEXT_H_

#include <cstdint>

typedef void *  fcontext_t;

extern "C" 
intptr_t jump_fcontext(fcontext_t *ofc, fcontext_t nfc,
                                               intptr_t vp, bool preserve_fpu = false);
extern "C" 
fcontext_t make_fcontext(void * sp, std::size_t size, void (*fn)(intptr_t));

#endif
