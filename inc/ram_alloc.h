/* 
 * File:   ram_alloc.h
 * Author: khoai
 *
 * Created on October 28, 2014, 10:28 AM
 */

#ifndef RAM_ALLOC_H
#define	RAM_ALLOC_H

#include "interface.h"

void* ram_malloc(size_t size);
void ram_free(void *ptr);

struct zalloc ram_handler = {
	.name = "ram",
	.malloc = ram_malloc,
	.free = ram_free,
};

#endif	/* RAM_ALLOC_H */

