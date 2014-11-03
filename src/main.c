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
#include <stdint.h>
#include <math.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "handler.h"

/*
 * 
 */

#define ALLOC_TOTAL 16000
#define ALLOC_SIZE	4096

#define IS_FREE 0

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

void limit_config() {
	struct rlimit limit;
	limit.rlim_max = limit.rlim_cur = 26;
	if (setrlimit(RLIMIT_AS, &limit)) {
		printf("get limit fail: %s\n", strerror(errno));
		return;
	}

	if (getrlimit(RLIMIT_AS, &limit)) {
		printf("get limit fail: %s\n", strerror(errno));
		return;
	}

	printf("limit: %dmb\n", (int) (1 << limit.rlim_max) / (1024 * 1024));
}

void wait_command() {
	char buffer[512];
	char *dummy = fgets(buffer, sizeof (buffer), stdin);
	return;
}

int main(int argc, char** argv) {
	limit_config();
	//	test_all();
	//--- test region
	if (1) {
		struct zalloc *zac = handler_get("buddy");
		if (!zac) {
			printf("allocate handler not found\n");
			return 1;
		}

		// small test
		int i;
		for (i = 0; i < 10; i++) {
			uint64_t sz = 1 << (i + 1);
			char *mem = zac->malloc(sz);
			if (!mem) {
				printf("out of memory\n");
				break;
			}

			printf("malloc(%ld)\n", sz);
			strcpy(mem, "blah blah blah");
			//zac->free(mem);
		}

		// huge test
		uint64_t huge_size = 1 << 26; // 64mb
		for (i = 0; i < 30; i++) {
			char *mem = zac->malloc(huge_size);
			if (!mem) {
				printf("out of memory\n");
				break;
			}

			printf("malloc(%ld)\n", huge_size);
			memset(mem, i, 100);
			//zac->free(mem);
		}
	}
	printf("done\n");
	wait_command();
	return (EXIT_SUCCESS);
}

#if LIB
// for zalloc library

void* malloc(size_t size) {
	struct zalloc *zac = handler_get("buddy");
	printf("malloc: %ld\n", size);
	void *ptr = zac->malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

void free(void *ptr) {
	// filter NULL pointer
	if (!ptr)
		return;
	struct zalloc *zac = handler_get("buddy");
	//	printf("free: %lx\n", (unsigned long) ptr);
	return zac->free(ptr);
}

void *realloc(void *ptr, size_t size) {
	free(ptr);
	return malloc(size);
}

void* calloc(size_t num, size_t size) {
	struct zalloc *zac = handler_get("buddy");
	printf("calloc: num(%ld), size(%ld)\n", num, size);
	return malloc(num * size);
}

int posix_memalign(void **ptr, size_t alignment, size_t size) {
	printf(" <!> posix_memalign\n");
	return 0;
}

void *aligned_alloc(size_t alignment, size_t size) {
	printf(" <!> aligned_alloc\n");
	return 0;
}

#endif