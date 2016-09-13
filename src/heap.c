#include <common/heap.h>

static size_t roundup_pow2(size_t x)
{
	int i;
	x -= 1;
	for (i = 1; i < 8 * (int)sizeof(x); i *= 2)
		x |= x >> i;
	return x + 1;
}

int heap_expand(struct heap *heap, size_t more_items)
{
	size_t request, size;
	void *newptr;

	/* we can already satisfy the request */
	request = heap->num_items + more_items;
	if (request <= heap->max_items)
		return 0;

	/* compute the new size */
	size = roundup_pow2(request);
	if (size < request)
		return 1;
	if (size < 4)
		size = 4;

	/* expand the heap */
	newptr = realloc(heap->items, size * sizeof(struct heap_item *));
	if (newptr == NULL)
		return 1;

	/* update the pointer */
	heap->items = newptr;
	heap->max_items = size;
	return 0;
}

void heap_compact(struct heap *heap, size_t max_items)
{
	void *newptr;

	if (max_items < heap->num_items)
		max_items = heap->num_items;

	if (max_items == heap->max_items)
		return;

	if (max_items == 0) {
		heap_destroy(heap);
		return;
	}

	newptr = realloc(heap->items, max_items * sizeof(struct heap_item *));
	if (newptr == NULL)
		return;
	heap->items = newptr;
	heap->max_items = max_items;
}
