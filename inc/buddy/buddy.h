// ref: https://github.com/cloudwu/buddy

/* 
 * File:   buddy.h
 * Author: khoai
 *
 * Created on October 29, 2014, 4:30 PM
 */

#ifndef BUDDY_H
#define	BUDDY_H

#include <stdint.h>

struct buddy;

struct buddy * buddy_new(int level);
void buddy_delete(struct buddy *);
uint64_t buddy_alloc(struct buddy *, uint64_t size);
void buddy_free(struct buddy *, uint64_t offset);
uint64_t buddy_size(struct buddy *, uint64_t offset);
void buddy_dump(struct buddy *);

#endif	/* BUDDY_H */

