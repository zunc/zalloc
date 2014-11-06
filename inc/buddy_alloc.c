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
#include "foot.h"
#include "common/dbg.h"

// map region
#define MAP_PATH "/home/khoai/Desktop/research/alloc/box/buddy.blo"
char *memblock;
int fd;
struct stat sb;

// buddy region
static unsigned buddy_level = 31; // 2^26 = 67108864
struct buddy *buddy;

struct zalloc buddy_handler = {
	.name = "buddy",
	.malloc = buddy_xmalloc,
	.free = buddy_xfree,
	.get_size = buddy_get_size,
	.get_page_size = buddy_get_page_size
};

// currently bolt-neck by one mutex
//pthread_mutex_t lock;

int isInit = 0;
void init_();

void* buddy_xmalloc(size_t size) {
	init_();
	if (!buddy)
		return NULL;
	uint64_t pos = buddy_alloc(buddy, size);
	if (pos < 0)
		return NULL;
	//	printf(" - virtual_pointer(0x%lx), pos_alloc(0x%lx)\n", (uint64_t) memblock + pos, pos);
	return (memblock + pos);
}

void buddy_xfree(void *ptr) {
	init_();
	if (!buddy)
		return;
	assert((uint64_t) ptr >= (uint64_t) memblock);
	buddy_free(buddy, (ptr - (void*) memblock));
}

size_t buddy_get_size(void *ptr) {
	init_();
	if (!buddy)
		return 0;
	size_t used = buddy_used_size(buddy, (ptr - (void*) memblock));
	return used;
}

size_t buddy_get_page_size(void *ptr) {
	init_();
	if (!buddy)
		return 0;
	size_t used = buddy_page_size(buddy, (ptr - (void*) memblock));
	return used;
}

void init_() {
	if (!isInit) {
		//		foot_init();
		printf("[+] buddy allocate init\n");
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
		if (sb.st_size < map_size) {
			printf("fail. sb.st_size(%ld) < map_size(%ld)\n", sb.st_size, map_size);
			return;
		}
		memblock = mmap(NULL, map_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
		log_info(" [+] memblock(%px), map_size(0x%lx)", memblock, map_size);
		if (memblock == MAP_FAILED) {
			printf("fail. mmap\n");
			return;
		}
		handler_register(&buddy_handler);
		isInit = 1;
	}
}

__attribute__((constructor))
static void __buddy_handler_init(void) {
	init_();
}

int isDeinit = 0;

__attribute__((destructor))
static void __buddy_handler_deinit(void) {
	printf("[+] buddy allocate deinit\n");
	if (!isDeinit) {
		isDeinit = 1;
		//	pthread_mutex_destroy(&lock);
		buddy_delete(buddy);
		uint64_t map_size = pow(2, buddy_level);
		log_info("memblock(%px), map_size(0x%lx)", memblock, map_size);
		munmap(memblock, map_size); //<!> some app bug
		close(fd);
		buddy = NULL;
	}
}