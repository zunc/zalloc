#include <stdlib.h>
#include "handler.h"
#include "ram_alloc.h"

void* ram_malloc(size_t size) {
	return malloc(size);
}

void ram_free(void *ptr) {
	free(ptr);
}

__attribute__((constructor))
static void __ram_handler_init(void) {
	handler_register(&ram_handler);
}