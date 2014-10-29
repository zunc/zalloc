#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "handler.h"
#include "mmap_alloc.h"

#define MAP_PATH "/home/khoai/Desktop/research/alloc/box/ram.blo"
#define MAP_SIZE 64*1024*1024

char *memblock;
int fd;
struct stat sb;

uint64_t pos_;
uint64_t remain_;

void* mmap_malloc(size_t size) {
	if (size > remain_) {
//		printf("out of memory\n");
		return NULL;
//		// reset
//		pos_ = 0;
//		remain_ = sb.st_size;
	}
	
	void *res = (void*) (memblock + pos_);
	pos_ += size;
	remain_ -= size;
	return res;
}

void mmap_free(void *ptr) {
//	free(ptr);
}

__attribute__((constructor))
static void __mmap_handler_init(void) {
	handler_register(&mmap_handler);
	///
	fd = open(MAP_PATH, O_RDWR);
	fstat(fd, &sb);

	memblock = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (memblock == MAP_FAILED)
		perror("mmap");
	
	pos_ = 0;
	remain_ = sb.st_size;
	
	char *last_rc = memblock + sb.st_size - 20;
	sprintf(last_rc, "last_pos: %d\n", 4);
}

__attribute__((destructor))
static void __mmap_handler_deinit() {
	munmap(memblock, sb.st_size);
	close(fd);
}