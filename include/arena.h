#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC malloc
#endif
#ifndef ARENA_FREE
#include <stdlib.h>
#define ARENA_FREE free
#endif
#ifndef ARENA_ASSERT
#include <assert.h>
#define ARENA_ASSERT assert
#endif

typedef struct ArenaNode ArenaNode;
struct ArenaNode {
	ArenaNode *next;
	ArenaNode *prev;
	void *d;
	size_t size;
	size_t cur;
};
typedef struct {
	ArenaNode *head;
	ArenaNode *tail;
} ArenaContext;

ArenaContext *arena_create(size_t chunksize, size_t chunks);
void arena_destroy(ArenaContext *ctx);

void arena_push(ArenaContext *ctx, size_t chunksize);
void arena_pop(ArenaContext *ctx);

void *arena_alloc(ArenaContext *ctx, size_t size, size_t align);

#define ARENA_NEW(ctx, type, count) \
	(type *)arena_alloc(ctx, sizeof(type) * count, alignof(type))

#endif // ARENA_H
