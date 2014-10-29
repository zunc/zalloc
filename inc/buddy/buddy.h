// ref: https://github.com/cloudwu/buddy

/* 
 * File:   buddy.h
 * Author: khoai
 *
 * Created on October 29, 2014, 4:30 PM
 */

#ifndef BUDDY_H
#define	BUDDY_H

struct buddy;

struct buddy * buddy_new(int level);
void buddy_delete(struct buddy *);
int buddy_alloc(struct buddy *, int size);
void buddy_free(struct buddy *, int offset);
int buddy_size(struct buddy *, int offset);
void buddy_dump(struct buddy *);

#endif	/* BUDDY_H */

