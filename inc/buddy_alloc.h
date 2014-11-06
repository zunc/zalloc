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
size_t buddy_get_size(void *ptr);
size_t buddy_get_page_size(void *ptr);

extern int isDeinit;

#endif	/* BUDDY_ALLOC_H */

