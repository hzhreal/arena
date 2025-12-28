#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/arena.h"

#define IS_POS_POW_OF_TWO(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

static inline ArenaNode *alloc_node(void)
{
	ArenaNode *node = ARENA_MALLOC(sizeof(*node));
	ARENA_ASSERT(node != NULL);
	return node;
}

static inline void fill_node(ArenaNode *node, size_t chunksize)
{
	ARENA_ASSERT(node != NULL);
	node->d = ARENA_MALLOC(chunksize);
	ARENA_ASSERT(node->d);
	node->size = chunksize;
	node->cur = 0;
}

ArenaContext *arena_create(size_t chunksize, size_t chunks)
{
	ArenaContext *ctx;
	ArenaNode *cur, *node;
	typeof(chunks) i;

	ctx = ARENA_MALLOC(sizeof(*ctx));
	ARENA_ASSERT(ctx != NULL);

	if (chunks == 0) {
		ctx->head = NULL;
		ctx->tail = NULL;
		return ctx;
	}

	ctx->head = alloc_node();
	ctx->head->prev = NULL;
	if (chunks == 1) {
		fill_node(ctx->head, chunksize);
		ctx->head->next = NULL;
		ctx->tail = ctx->head;
		return ctx;
	}

	cur = ctx->head;
	for (i = 0; i < chunks - 2; i++) {
		fill_node(cur, chunksize);
		node = alloc_node();
		node->prev = cur;
		cur->next = node;
		cur = cur->next;
	}
	fill_node(cur, chunksize);
	cur->next = NULL;
	ctx->tail = cur;

	return ctx;
}

void arena_destroy(ArenaContext *ctx)
{
	ARENA_ASSERT(ctx != NULL);

	ArenaNode *cur = ctx->head;
	ArenaNode *next;
	while (cur != NULL) {
		next = cur->next;
		ARENA_FREE(cur->d);
		ARENA_FREE(cur);
		cur = next;
	}
	ARENA_FREE(ctx);
}

void arena_push(ArenaContext *ctx, size_t chunksize)
{
	ARENA_ASSERT(ctx != NULL);

	ArenaNode *node = alloc_node();
	fill_node(node, chunksize);
	ctx->tail->next = node;
	node->next = NULL;
	node->prev = ctx->tail;
	ctx->tail = node;
}

void arena_pop(ArenaContext *ctx)
{
	ARENA_ASSERT(ctx != NULL);
	ARENA_ASSERT(ctx->head != NULL && ctx->tail != NULL);

	ArenaNode *old_tail = ctx->tail;
	ctx->tail = old_tail->prev;

	if (ctx->tail == NULL)
		ctx->head = NULL;
	else
		ctx->tail->next = NULL;

	ARENA_FREE(old_tail->d);
	ARENA_FREE(old_tail);
}

void *arena_alloc(ArenaContext *ctx, size_t size, size_t align)
{
	ARENA_ASSERT(ctx != NULL && ctx->head != NULL);
	ARENA_ASSERT(IS_POS_POW_OF_TWO(align));

	if (size == 0) {
		return NULL;
	}

	ArenaNode *head = ctx->head;
	ArenaNode *i;
	uintptr_t a_addr, addr;
	ptrdiff_t addr_diff;

	for (i = head; i != NULL; i = i->next) {
		addr = (uintptr_t)i->d + i->cur;
		a_addr = ALIGN_UP(addr, align);
		addr_diff = a_addr - addr;
		if (i->cur + addr_diff + size > i->size)
			continue;
		i->cur += addr_diff + size;
		return (void *)a_addr;
	}
	arena_push(ctx, size);
	return ctx->tail->d;
}
