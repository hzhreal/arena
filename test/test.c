#include <stdint.h>

#include "../include/arena.h"

#define CHUNKSIZE 1000

int main(void)
{
	ArenaContext *ctx = arena_create(CHUNKSIZE, 2);
	uint8_t *d = ARENA_NEW(ctx, uint8_t, 50);
	*(d + 5) = 24;
	for (int i = 0; i < 50; i++) {
		arena_push(ctx, 100);
		if (i % 2 == 0)
			arena_pop(ctx);
	}
	int *n = ARENA_NEW(ctx, int, 2000);
	(void)n;
	arena_destroy(ctx);
	return 0;
}
