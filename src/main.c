/* 
 * File:   main.c
 * Author: khoai
 *
 * Created on October 28, 2014, 10:15 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

#include "handler.h"

/*
 * 
 */

//void timeit(int (*cb_func)(int, int), const char* func_name) {
//    struct timespec start;
//    struct timespec end;
//
//    int nFrom = 1;
//    int nTo = 100 * 1000000;
//    long span;
//    int ret = 0;
//    
//    clock_gettime(CLOCK_REALTIME, &start);
//    ret = cb_func(nFrom, nTo);
//    clock_gettime(CLOCK_REALTIME, &end);
//
//    span = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);
//    printf("%s: %d -> %ldns\n", func_name, ret, span);
//}

#define ALLOC_TOTAL 1000000
#define ALLOC_SIZE	10000

#define IS_FREE 1

void timeit(struct zalloc *h) {
	struct timespec start;
	struct timespec end;
	long span;
	int i = 0;
	void *ret;

	clock_gettime(CLOCK_REALTIME, &start);
	for (i = 0; i < ALLOC_TOTAL; i++) {
		ret = h->malloc(ALLOC_SIZE);
		if (!ret) {
			printf("\tout of memory, step(%d)\n", i);
			break;
		}
//		memset(ret, 'a', ALLOC_SIZE);
		strcpy(ret, "123456789");
		if (IS_FREE)
			h->free(ret);
	}
	clock_gettime(CLOCK_REALTIME, &end);

	span = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);
	printf(" \t-> %ldns, %ldms\n", span, (long int) (span / 1000000));
}

void test_all() {
	struct zalloc *h;
	printf("[] total(%d), alloc_size(%d)\n", ALLOC_TOTAL, ALLOC_SIZE);
	struct list *handlers = handlers_get();

	list_for_each_entry(h, handlers, list) {
		printf("> %s\n", h->name);
		timeit(h);
	}
}

int main(int argc, char** argv) {
//	test_all();
//	//--- test region
	struct zalloc *zac = handler_get("buddy");
	if (!zac) {
		printf("allocate handler not found\n");
		return 1;
	}
	int i;
	for (i = 0; i < 66; i++) {
		int sz = 100;//1 << (i+1);
		char *mem = zac->malloc(sz);
		if (!mem) {
			printf("out of memory\n");
			break;
		}

		printf("malloc(%d)\n", sz);
		strcpy(mem, "blah blah blah");
//		zac->free(mem);
	}
	return (EXIT_SUCCESS);
}
