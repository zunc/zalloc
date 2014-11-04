#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

#include "handler.h"
#include "buddy_alloc.h"
#include "buddy/buddy.h"

// map region
#define MAP_PATH "/home/khoai/Desktop/research/alloc/box/buddy.blo"
char *memblock;
int fd;
struct stat sb;

// buddy region
static unsigned buddy_level = 31; // 2^26 = 67108864
struct buddy *buddy;

// currently bolt-neck by one mutex
pthread_mutex_t lock;

void* buddy_xmalloc(size_t size) {
	pthread_mutex_lock(&lock);
	uint64_t pos = buddy_alloc(buddy, size);
	if (pos < 0) {
		pthread_mutex_unlock(&lock);
		return NULL;
	}
//	printf(" - virtual_pointer(0x%lx), pos_alloc(0x%lx)\n", (uint64_t) memblock + pos, pos);
	pthread_mutex_unlock(&lock);
	return (memblock + pos);
}

void buddy_xfree(void *ptr) {
	pthread_mutex_lock(&lock);
//	printf(" - free(0x%lx)\n", (uint64_t) ptr);
	assert((uint64_t) ptr > (uint64_t) memblock);
	buddy_free(buddy, (ptr - (void*) memblock));
	pthread_mutex_unlock(&lock);
}

size_t buddy_get_size(void *ptr) {
	size_t used = buddy_used_size(buddy, (ptr - (void*) memblock));
	return used;
}

__attribute__((constructor))
static void __buddy_handler_init(void) {
	printf("buddy allocate init\n");
	buddy = buddy_new(buddy_level);
	int size = 1 << buddy_level;

	// mmap
	if ((fd = open(MAP_PATH, O_RDWR | O_CREAT)) == 0) {
		perror("__buddy_handler_init. open file fail\n");
		return;
	}

	fstat(fd, &sb);
	if (size > sb.st_size) {
		printf("file size smaller buffer size\n");
		return;
	}

	uint64_t map_size = pow(2, buddy_level);
	memblock = mmap(NULL, map_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (memblock == MAP_FAILED) {
		printf("fail. mmap\n");
		return;
	}

	// lock init
	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("mutex init failed\n");
		return;
	}
	printf("init mutex\n");

	handler_register(&buddy_handler);
}

__attribute__((destructor))
static void __buddy_handler_deinit(void) {
	pthread_mutex_destroy(&lock);
	buddy_delete(buddy);
	munmap(memblock, sb.st_size);
	close(fd);
}