/* 
 * File:   slab_alloc.h
 * Author: khoai
 *
 * Created on October 29, 2014, 2:29 PM
 */

#ifndef SLAB_ALLOC_H
#define	SLAB_ALLOC_H

#include "interface.h"

void* slab_xmalloc(size_t size);
void slab_xfree(void *ptr);

struct zalloc slab_handler = {
	.name = "slab",
	.malloc = slab_xmalloc,
	.free = slab_xfree,
};

#endif	/* SLAB_ALLOC_H */

