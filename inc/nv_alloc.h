/* 
 * File:   nv_alloc.h
 * Author: khoai
 *
 * Created on October 29, 2014, 11:36 AM
 */

#ifndef NV_ALLOC_H
#define	NV_ALLOC_H

void* nv_malloc(size_t size);
void nv_free(void *ptr);

struct zalloc nv_handler = {
	.name = "nv",
	.malloc = nv_malloc,
	.free = nv_free,
};

#endif	/* NV_ALLOC_H */

