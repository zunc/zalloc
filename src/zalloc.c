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
#include <assert.h>
#include "handler.h"
#include "common/dbg.h"
#include "buddy_alloc.h"

struct zalloc buddy_handler;
/*
 * 
 */

#define ALLOC_TOTAL 16000
#define ALLOC_SIZE	4096

#define IS_FREE 0
#define IS_LOG 0

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
	if (setrlimit(RLIMIT_DATA, &limit)) {
		printf("get limit fail: %s\n", strerror(errno));
		return;
	}

	if (getrlimit(RLIMIT_DATA, &limit)) {
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

#if !LIB
void *realloc(void *ptr, size_t size);
int main(int argc, char** argv) {
	limit_config();
	//	test_all();
	//--- realloc test
	if (1) {
		int max_pow = 26;
		int i;
		struct zalloc *zac = handler_get("buddy");
		assert(zac);
		char *mem = NULL;
		for (i=0; i < max_pow; i++) {
			int size = 1 << i;
			log_info("realloc: %d", size);
			mem = realloc(mem, size);
			if (!mem)
				log_err("toi");
		}
	}
	
	//--- overlap test
	if (0) {
		int sz1 = 0x1004;
		int sz2 = 0x1008;
		struct zalloc *zac = handler_get("buddy");
		assert(zac);
		char *ptr1 = zac->malloc(sz1);
		char *ptr2 = zac->malloc(sz2);
		printf(" - ptr1(0x%lx)\n", (uint64_t) ptr1);
		printf(" - ptr2(0x%lx)\n", (uint64_t) ptr2);
		printf(" -> range(0x%lx)\n", (uint64_t) abs(ptr1 - ptr2));
	}

	//--- test region
	if (0) {
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

			size_t used_size = zac->get_size(mem);
			printf("malloc(%ld), used_size(%ld)\n", sz, used_size);
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
			//			zac->free(mem);
		}
	}
	printf("done\n");
	wait_command();
	return (EXIT_SUCCESS);
}
#endif

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
// <!> don't select ram mode
pthread_mutex_t lock_alloc;
void *realloc(void *ptr, size_t size) {
//	log_warn(" <!> realloc: ptr(0x%0lx), size(%ld)", (uint64_t) ptr, size);
	pthread_mutex_lock(&lock_alloc);
	void *new_ptr = buddy_xmalloc(size);
	if (new_ptr) {
		if (ptr) {
			size_t old_size = buddy_get_size(ptr);
			size_t size_copy = MIN(size, old_size);
			memcpy(new_ptr, ptr, size_copy);
			buddy_xfree(ptr);
			log_dbg(" \t -> 0x%lx, old_size(%ld)", (uint64_t) new_ptr, old_size);
		}
	}
	pthread_mutex_unlock(&lock_alloc);
	return new_ptr;
}

#if LIB
// for zalloc library
void* malloc(size_t size) {
	pthread_mutex_lock(&lock_alloc);
	void *ptr = buddy_xmalloc(size);
	log_dbg("malloc: 0x%ld -> 0x%lx", size, (uint64_t) ptr);
	pthread_mutex_unlock(&lock_alloc);
	return ptr;
}

int isDeinit;

void free(void *ptr) {
	// filter NULL pointer
	if (!ptr)
		return;
	
	pthread_mutex_lock(&lock_alloc);
	//	assert(zac);
	if (isDeinit) {
		log_err("free: 0x%lx", (unsigned long) ptr);
	} else {
		log_dbg("free: 0x%lx", (unsigned long) ptr);
	}
	buddy_xfree(ptr);
	pthread_mutex_unlock(&lock_alloc);

}

void* calloc(size_t num, size_t size) {
	pthread_mutex_lock(&lock_alloc);
	log_dbg("calloc: num(%ld), size(%ld)", num, size);
	void *ptr = buddy_xmalloc(num * size);
	memset(ptr, 0, num * size);
	pthread_mutex_unlock(&lock_alloc);
	return ptr;
}

int posix_memalign(void **res, size_t align, size_t len) {
	// lazy coding
	log_err(" <!> posix_memalign: align(%lx), size(%ld)", align, len);
	void *ptr = malloc(len);
	if (!ptr)
		return 1;
	*res = ptr;
	return 0;
}

void *memalign(size_t align, size_t len) {
	void *mem;
	int ret;
	if ((ret = posix_memalign(&mem, align, len))) {
		errno = ret;
		return 0;
	}
	return mem;
}

void *aligned_alloc(size_t align, size_t len) {
	log_err(" <!> aligned_alloc: align(%lx), size(%ld)", align, len);
	return NULL;
}

__attribute__((constructor))
static void __zalloc_lib_init(void) {
	if (pthread_mutex_init(&lock_alloc, NULL) != 0) {
		log_dbg("mutex init failed");
		return;
	}
	log_dbg("init mutex");
}

__attribute__((destructor))
static void __zalloc_lib_deinit(void) {
	pthread_mutex_destroy(&lock_alloc);
	log_dbg("destroy mutex");
}

#endif