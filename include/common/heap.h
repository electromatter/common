#ifndef COMMON_HEAP_H
#define COMMON_HEAP_H

#include <assert.h>

static inline size_t round_down_pow2(size_t index)
{
	index |= index >> 1;
	index |= index >> 2;
	index |= index >> 4;
	index |= index >> 8;
	index |= index >> 16;
	index |= index >> 32;
	return index ^ (index >> 1);
}

/* heap_type - type of the container
 * type - type of values
 * COUNT(heap) - returns the number of items in the container
 * SWAP(heap, left_index, right_index) - swaps two items in the container
 * TEST(heap, parent_index, child_index) - returns zero if violated
 *	for min-heaps: parent <= child
 *	for max-heaps: parent >= child
 * POP(heap) - removes the last item from the container and returns it */
#define GEN_HEAP_BASE(attr, prefix, heap_type, type, COUNT, SWAP,	\
			TEST, POP)					\
attr void								\
prefix##siftup(heap_type *heap, size_t index) {				\
	size_t parent;							\
	if (index >= COUNT(heap))					\
		return;							\
	while (index > 0) {						\
		parent = (index - 1) / 2;				\
		if (TEST(heap, parent, index))				\
			break;						\
		SWAP(heap, parent, index);				\
		index = parent;						\
	}								\
}									\
attr void								\
prefix##siftdown(heap_type *heap, size_t index) {			\
	size_t child, count = COUNT(heap);				\
	while ((child = 2 * index + 1) < count) {			\
		if (child + 1 < count) {				\
			if (!TEST(heap, child, child + 1))		\
				child += 1;				\
		}							\
		if (test(heap, index, child))				\
			break;						\
		SWAP(heap, index, child);				\
		index = child;						\
	}								\
}									\
attr void								\
prefix##heapify(heap_type *heap) {					\
	size_t index = COUNT(heap);					\
	if (index <= 1)							\
		return;							\
	index = round_down_pow2(index) - 2;				\
	while (index --> 0)						\
		prefix##siftdown(heap, index);				\
}									\
attr void								\
prefix##remove(heap_type *heap, size_t index, type *val) {		\
	size_t last = COUNT(heap) - 1;					\
	assert(index <= last);						\
	if (index != last)						\
		SWAP(heap, index, last);				\
	if (val != NULL)						\
		*val = POP(heap);					\
	else								\
		POP(heap);						\
	prefix##siftup(heap, index);					\
	prefix##siftdown(heap, index);					\
}

#endif

