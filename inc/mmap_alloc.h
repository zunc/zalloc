/* 
 * File:   mmap_alloc.h
 * Author: khoai
 *
 * Created on October 28, 2014, 11:23 AM
 */

#ifndef MMAP_ALLOC_H
#define	MMAP_ALLOC_H

void* mmap_malloc(size_t size);
void mmap_free(void *ptr);

struct zalloc mmap_handler = {
	.name = "mmap",
	.malloc = mmap_malloc,
	.free = mmap_free,
};

#endif	/* MMAP_ALLOC_H */

