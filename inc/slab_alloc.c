#include <stdlib.h>
#include "handler.h"
#include "slab_alloc.h"
#include "slab/slab.h"

static unsigned size = 64 * 1024 * 1024;
struct slab_chain slab;
#define SLAB_SIZE 128

void* slab_xmalloc(size_t size) {
	if (size > SLAB_SIZE)
		return NULL;
	return slab_alloc(&slab);
}

void slab_xfree(void *ptr) {
	slab_free(&slab, ptr);
}

__attribute__((constructor))
static void __nv_handler_init(void) {
//	slab_init(&slab, SLAB_SIZE);
//	handler_register(&slab_handler);
}

__attribute__((destructor))
static void __nv_handler_deinit(void) {
//	slab_destroy(&slab);
}