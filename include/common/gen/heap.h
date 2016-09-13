#ifndef COMMON_HEAP_H
#define COMMON_HEAP_H

#include <stdlib.h>

#ifndef containerof
#include <stddef.h>
#include <sys/types.h>

static inline void *psub_or_null(const void *ptr, ssize_t delta)
{
	if (ptr == NULL)
		return NULL;
	return (char *)ptr - delta;
}

#define containerof(ptr, type, member)					\
	((type *)psub_or_null((0 ? &((type *)NULL)->member : (ptr)),	\
			offsetof(type, member)))
#endif

struct heap_item {
	size_t index;
};

struct heap {
	struct heap_item **items;
	size_t num_items, max_items;
};

/* returns non-zero if heap property is satisfied
 * parent <= child for a min heap
 * parent >= child for a max heap */
typedef int (*heaptest_t)(const struct heap_item *parent, const struct heap_item *child);

#define HEAP_INIT		{NULL, 0, 0}

int heap_expand(struct heap *heap, size_t more_items);
void heap_compact(struct heap *heap, size_t max_items);

static inline
void heap_init(struct heap *heap)
{
	heap->items = NULL;
	heap->num_items = 0;
	heap->max_items = 0;
}

static inline
void heap_destroy(struct heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->num_items = 0;
	heap->max_items = 0;
}

static inline
void heap_sift_down(struct heap *heap, struct heap_item *item, heaptest_t cmp)
{
	size_t index, child_index;
	struct heap_item *child;

	index = item->index;
	while (1) {
		child_index = 2 * index + 1;
		if (child_index >= heap->num_items)
			break;
		child = heap->items[child_index];

		/* pick the child that would satisfy the heap property */
		if (child_index + 1 < heap->num_items) {
			if (!cmp(child, heap->items[child_index + 1])) {
				child_index = child_index + 1;
				child = heap->items[child_index];
			}
		}

		/* check if the heap is already valid. */
		if (cmp(item, child))
			break;

		/* reapair the heap and continue down. */
		heap->items[child_index] = item;
		heap->items[index] = child;
		item->index = child_index;
		child->index = index;
		index = child_index;
	}
}

static inline
void heap_sift_up(struct heap *heap, struct heap_item *item, heaptest_t cmp)
{
	size_t index, parent_index;
	struct heap_item *parent;

	index = item->index;
	while (index > 0) {
		parent_index = (index - 1) / 2;
		parent = heap->items[parent_index];

		/* check if the heap is already valid. */
		if (cmp(parent, item))
			break;

		/* repair the heap and continue up. */
		heap->items[parent_index] = item;
		heap->items[index] = parent;
		item->index = parent_index;
		parent->index = index;
		index = parent_index;
	}
}

static inline
struct heap_item *heap_peek(struct heap *heap)
{
	if (heap->num_items <= 0)
		return NULL;
	return heap->items[0];
}

static inline
void heap_remove(struct heap *heap, struct heap_item *item, heaptest_t cmp)
{
	struct heap_item *succ;
	heap->num_items -= 1;
	if (heap->num_items == item->index)
		return;
	succ = heap->items[heap->num_items];
	heap->items[item->index] = succ;
	heap_sift_up(heap, succ, cmp);
	heap_sift_down(heap, succ, cmp);
}

static inline
int heap_push(struct heap *heap, struct heap_item *item, heaptest_t cmp)
{
	if (heap_expand(heap, 1))
		return 1;
	item->index = heap->num_items;
	heap->items[item->index] = item;
	heap->num_items += 1;
	heap_sift_up(heap, item, cmp);
	return 0;
}

static inline
struct heap_item *heap_pop(struct heap *heap, heaptest_t cmp)
{
	struct heap_item *item, *succ;
	if (heap->num_items <= 0)
		return NULL;
	item = heap->items[0];
	heap->num_items -= 1;
	if (heap->num_items == item->index)
		return item;
	succ = heap->items[heap->num_items];
	heap->items[item->index] = succ;
	succ->index = item->index;
	heap_sift_down(heap, succ, cmp);
	return item;
}

#endif
