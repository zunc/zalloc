#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>
#include <pthread.h>
#include "foot.h"

typedef void* (*func_malloc)(size_t size);
typedef void (*func_free)(void *ptr);

func_malloc _origin_malloc;
func_free _origin_free;

pthread_mutex_t lock_foot;

void* origin_malloc(size_t size) {
	assert(_origin_malloc);
	return _origin_malloc(size);
}

void origin_free(void *ptr) {
	assert(_origin_free);
	return _origin_free(ptr);
}

int isFootInit = 0;

void foot_init() {
	pthread_mutex_lock(&lock_foot);
	if (!isFootInit) {
		printf("[+] foot init\n");
		char *msg = NULL;
		// malloc
		if (_origin_malloc == NULL) {
			char fname[] = "malloc";
			_origin_malloc = dlsym(RTLD_NEXT, fname);
			printf("old_%s = 0x%px\n", fname, _origin_malloc);
			if ((_origin_malloc == NULL) || ((msg = dlerror()) != NULL)) {
				printf("dlsym [%s] failed : %s\n", fname, msg);
				goto clean;
			} else {
				printf("dlsym: wrapping [%s] done\n", fname);
			}
		}

		// free
		if (_origin_free == NULL) {
			char fname[] = "free";
			_origin_free = dlsym(RTLD_NEXT, fname);
			printf("old_%s = 0x%px\n", fname, _origin_free);
			if ((_origin_free == NULL) || ((msg = dlerror()) != NULL)) {
				printf("dlsym [%s] failed : %s\n", fname, msg);
				goto clean;
			} else {
				printf("dlsym: wrapping [%s] done\n", fname);
			}
		}
		isFootInit = 1;
	}

clean:
	pthread_mutex_unlock(&lock_foot);
}

__attribute__((constructor))
static void __foot_init(void) {
//	foot_init();
}
//
//__attribute__((destructor))
//static void __foot_deinit(void) {
//	printf("[+] foot deinit\n");
//}