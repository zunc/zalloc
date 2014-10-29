#include <stdlib.h>
#include "handler.h"
#include "nv_alloc.h"
#include "nvmalloc/nvmalloc.h"

static unsigned size = 64 * 1024 * 1024;

void* nv_malloc(size_t size) {
	return nvmalloc(size);
}

void nv_free(void *ptr) {
	nvfree(ptr);
}

__attribute__((constructor))
static void __nv_handler_init(void) {
	nvmalloc_init(size / 4096, 0);
	handler_register(&nv_handler);
}

__attribute__((destructor))
static void __nv_handler_deinit(void) {
	nvmalloc_exit();
}