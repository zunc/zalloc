/* 
 * File:   interface.h
 * Author: khoai
 *
 * Created on October 28, 2014, 10:22 AM
 */

#ifndef INTERFACE_H
#define	INTERFACE_H

#include <stddef.h>
#include "common/mini-clist.h"

struct zalloc {
	char *name;
	void* (*malloc)(size_t size);
	void* (*calloc)(size_t number, size_t size);
	void* (*realloc)(void *ptr, size_t size);
	void (*free)(void *ptr);
	struct list list;
};

#endif	/* INTERFACE_H */

