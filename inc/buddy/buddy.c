#include "buddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define NODE_UNUSED 0
#define NODE_USED	1	
#define NODE_SPLIT	2
#define NODE_FULL	3

#define STATIC_ARRAY 0
// max node for 2^31 page node
#define MAX_NODE	1048575

#pragma pack(push)
#pragma pack(1)

struct node {
	uint32_t pages; // page count number
	uint64_t used; // used allocate size
	uint8_t state;
	// mutex
	//
};
#pragma pack(pop)

struct buddy {
	uint8_t level; // level node in tree
#if LIB
	struct node tree[MAX_NODE];
#else
	struct node tree[1];
#endif
};

#define PAGE_SIZE 4096

static inline int
is_pow_of_2(uint32_t x) {
	return !(x & (x - 1));
}

struct buddy self;

struct buddy *
buddy_new(int level) {
	printf("buddy_new: level(%d), page_level(%d)\n", level, (int) log2(PAGE_SIZE));
	if (!is_pow_of_2(PAGE_SIZE)) {
		printf("PAGE_SIZE is_not_pow_of_2\n");
		return NULL;
	}
	int level_of_page = log2(PAGE_SIZE);

	uint64_t size_total = pow(2, level);
	if (size_total < PAGE_SIZE) {
		printf("size_total(%ld) smaller PAGE_SIZE(%d)\n", size_total, PAGE_SIZE);
		return NULL;
	}

	uint64_t page_level = level - level_of_page;
	uint64_t page_total = 1 << page_level;
	uint64_t size_node = sizeof (uint8_t) * (1 << (page_level + 1)) - 1;
	printf(" - total_page(%ld), tree_node(%ld)\n", page_total, size_node);

#if LIB
	if (size_node > MAX_NODE) {
		printf("size_node(%ld) is lager than MAX_NODE(%d)\n", size_node, MAX_NODE);
		return NULL;
	}
	self.level = page_level;
	memset(self.tree, NODE_UNUSED, sizeof (struct node)*size_node);
	return &self;
#else
	struct buddy * self = malloc(sizeof (struct buddy) + sizeof (struct node)*(size_node - 1));
	self->level = page_level;
	memset(self->tree, NODE_UNUSED, sizeof (struct node) * size_node);
	return self;
#endif
}

void
buddy_delete(struct buddy * self) {
#if !LIB
	free(self);
#endif
}

static inline uint64_t
next_pow_of_2(uint64_t x) {
	if (is_pow_of_2(x))
		return x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	return x + 1;
}

static inline uint64_t
_index_offset(uint64_t index, int level, int max_level) {
	return PAGE_SIZE * (((index + 1) - (1 << level)) << (max_level - level));
}

static void
_mark_parent(struct buddy * self, uint64_t index) {
	for (;;) {
		int buddy = index - 1 + (index & 1) * 2;
		if (buddy > 0 && (self->tree[buddy].state == NODE_USED || self->tree[buddy].state == NODE_FULL)) {
			index = (index + 1) / 2 - 1;
			self->tree[index].state = NODE_FULL;
		} else {
			return;
		}
	}
}

uint64_t
buddy_alloc(struct buddy * self, uint64_t s) {
	uint64_t size = next_pow_of_2(ceil((float) s / PAGE_SIZE));
	if (size == 0)
		size = 1;

	uint64_t length = 1 << self->level;
	if (size > length)
		return -1;

	uint64_t index = 0;
	int level = 0;

	while (index >= 0) {
		if (size == length) {
			if (self->tree[index].state == NODE_UNUSED) {
				self->tree[index].state = NODE_USED;
				self->tree[index].used = s;
				self->tree[index].pages = size;
				_mark_parent(self, index);
				return _index_offset(index, level, self->level);
			}
		} else {
			// size < length
			switch (self->tree[index].state) {
				case NODE_USED:
				case NODE_FULL:
					break;
				case NODE_UNUSED:
					// split first
					self->tree[index].state = NODE_SPLIT;
					self->tree[index * 2 + 1].state = NODE_UNUSED;
					self->tree[index * 2 + 2].state = NODE_UNUSED;
				default:
					index = index * 2 + 1;
					length /= 2;
					level++;
					continue;
			}
		}
		if (index & 1) {
			++index;
			continue;
		}
		for (;;) {
			level--;
			length *= 2;
			index = (index + 1) / 2 - 1;
			if (index < 0)
				return -1;
			if (index & 1) {
				++index;
				break;
			}
		}
	}

	return -1;
}

static void
_combine(struct buddy * self, uint64_t index) {
	for (;;) {
		int buddy = index - 1 + (index & 1) * 2;
		if (buddy < 0 || self->tree[buddy].state != NODE_UNUSED) {
			self->tree[index].state = NODE_UNUSED;
			while (((index = (index + 1) / 2 - 1) >= 0) && self->tree[index].state == NODE_FULL) {
				self->tree[index].state = NODE_SPLIT;
			}
			return;
		}
		index = (index + 1) / 2 - 1;
	}
}

void
buddy_free(struct buddy * self, uint64_t _offset) {
	uint64_t offset = _offset / PAGE_SIZE;
	assert(offset < (1 << self->level));
	uint64_t left = 0;
	uint64_t length = 1 << self->level;
	uint64_t index = 0;

	for (;;) {
		switch (self->tree[index].state) {
			case NODE_USED:
				assert(offset == left);
				_combine(self, index);
				return;
			case NODE_UNUSED:
				assert(0);
				return;
			default:
				length /= 2;
				if (offset < left + length) {
					index = index * 2 + 1;
				} else {
					left += length;
					index = index * 2 + 2;
				}
				break;
		}
	}
}

uint64_t
buddy_page_size(struct buddy * self, uint64_t offset) {
	assert(offset < (1 << self->level));
	uint64_t left = 0;
	uint64_t length = 1 << self->level;
	uint64_t index = 0;

	for (;;) {
		switch (self->tree[index].state) {
			case NODE_USED:
				assert(offset == left);
				return length;
			case NODE_UNUSED:
				assert(0);
				return length;
			default:
				length /= 2;
				if (offset < left + length) {
					index = index * 2 + 1;
				} else {
					left += length;
					index = index * 2 + 2;
				}
				break;
		}
	}
}

uint64_t
buddy_used_size(struct buddy * self, uint64_t _offset) {
	uint64_t offset = _offset / PAGE_SIZE;
	assert(offset < (1 << self->level));
	uint64_t left = 0;
	uint64_t length = 1 << self->level;
	uint64_t index = 0;

	for (;;) {
		switch (self->tree[index].state) {
			case NODE_USED:
				assert(offset == left);
				return self->tree[index].used;
			case NODE_UNUSED:
				assert(0);
				return 0;
			default:
				length /= 2;
				if (offset < left + length) {
					index = index * 2 + 1;
				} else {
					left += length;
					index = index * 2 + 2;
				}
				break;
		}
	}
	return 0;
}

static void
_dump(struct buddy * self, uint64_t index, int level) {
	switch (self->tree[index].state) {
		case NODE_UNUSED:
			printf("(%ld:%d)", _index_offset(index, level, self->level), 1 << (self->level - level));
			break;
		case NODE_USED:
			printf("[%ld:%d]", _index_offset(index, level, self->level), 1 << (self->level - level));
			break;
		case NODE_FULL:
			printf("{");
			_dump(self, index * 2 + 1, level + 1);
			_dump(self, index * 2 + 2, level + 1);
			printf("}");
			break;
		default:
			printf("(");
			_dump(self, index * 2 + 1, level + 1);
			_dump(self, index * 2 + 2, level + 1);
			printf(")");
			break;
	}
}

void
buddy_dump(struct buddy * self) {
	_dump(self, 0, 0);
	printf("\n");
}
