#include <stdlib.h>
#include "handler.h"
#include "common/mini-clist.h"

static struct list handlers = LIST_HEAD_INIT(handlers);

void handler_register(struct zalloc *h) {
	LIST_ADDQ(&handlers, &h->list);
}

void handler_unregister(struct zalloc *hd) {
	LIST_DEL(&hd->list);
	LIST_INIT(&hd->list);
}

struct zalloc* handler_get(const char* name) {
	struct zalloc *h;
	list_for_each_entry(h, &handlers, list) {
		if (!strcmp(h->name, name))
			return h;
	}
	return 0;
}

struct list* handlers_get() {
	return &handlers;
}