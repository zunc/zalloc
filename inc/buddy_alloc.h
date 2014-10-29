/* 
 * File:   buddy_alloc.h
 * Author: khoai
 *
 * Created on October 29, 2014, 4:39 PM
 */

#ifndef BUDDY_ALLOC_H
#define	BUDDY_ALLOC_H

#include "interface.h"

void* buddy_xmalloc(size_t size);
void buddy_xfree(void *ptr);

struct zalloc buddy_handler = {
	.name = "buddy",
	.malloc = buddy_xmalloc,
	.free = buddy_xfree
};

#endif	/* BUDDY_ALLOC_H */

