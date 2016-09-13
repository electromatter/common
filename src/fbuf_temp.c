#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <mcp_base/fbuf.h>

#include <common/util.h>

#define MAX_TEMPBUF	(100)

static intptr_t depth = 0;
static int initalized = 0;
static struct fbuf storage[MAX_TEMPBUF];

static void tempbuf_init(void)
{
	int i;
	for (i = 0; i < MAX_TEMPBUF; i++)
		fbuf_init(&storage[i], FBUF_MAX);
	initalized = 1;
}

#include <stdio.h>
tempbuf_frame tempbuf_push(void)
{
	if (!initalized)
		tempbuf_init();
	return (tempbuf_frame)depth;
}

void tempbuf_pop(tempbuf_frame frame)
{
	intptr_t old_depth = (intptr_t)frame;
	if (old_depth > depth)
		abort();
	depth = old_depth;
}

struct fbuf *tempbuf(void)
{
	if (depth >= MAX_TEMPBUF)
		return NULL;
	fbuf_clear(&storage[depth]);
	return &storage[depth++];
}
