#include <stdio.h>

#include <common/heap.h>

typedef int heap_item_t;

typedef VEC_HEAP(, heap_item_t) heap_t;

#define test(parent, child) ((parent) >= (child))

HEAP_GEN(, heap_, heap_t, heap_item_t, test)

void show_heap(heap_t *heap)
{
	size_t i;
	printf("heap size: %li\n", VEC_COUNT(heap));
	for (i = 0; i < VEC_COUNT(heap); i++)
		printf("%i\n", VEC_AT(heap, i));
}

int main(void)
{
	heap_t heap = HEAP_INITIALIZER;

	if (heap_push(&heap, 0) < 0)
		return 1;
	if (heap_push(&heap, 1) < 0)
		return 1;
	if (heap_push(&heap, 2) < 0)
		return 1;
	if (heap_push(&heap, 5) < 0)
		return 1;
	if (heap_push(&heap, -1) < 0)
		return 1;

	show_heap(&heap);

	return 0;
}

