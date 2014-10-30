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

#include "handler.h"
#include "buddy_alloc.h"
#include "buddy/buddy.h"

// map region
#define MAP_PATH "/home/khoai/Desktop/research/alloc/box/buddy.blo"
char *memblock;
int fd;
struct stat sb;

// buddy region
static unsigned buddy_level = 16; // 2^26 = 67108864
struct buddy *buddy;

void* buddy_xmalloc(size_t size) {
	int pos = buddy_alloc(buddy, size);
	if (pos < 0) {
		return NULL;
	}
	return (memblock + pos);
}

void buddy_xfree(void *ptr) {
	buddy_free(buddy, (ptr - (void*) memblock));
}

__attribute__((constructor))
static void __buddy_handler_init(void) {
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

	memblock = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (memblock == MAP_FAILED)
		perror("mmap");

	handler_register(&buddy_handler);
}

__attribute__((destructor))
static void __buddy_handler_deinit(void) {
	buddy_delete(buddy);
	munmap(memblock, sb.st_size);
	close(fd);
}