/* 
 * File:   handler.h
 * Author: khoai
 *
 * Created on October 28, 2014, 10:37 AM
 */

#ifndef HANDLER_H
#define	HANDLER_H

#include "interface.h"

void handler_register(struct zalloc *handler);
void handler_unregister(struct zalloc *handler);
struct zalloc* handler_get(const char* name);
struct list* handlers_get();

#endif	/* HANDLER_H */

