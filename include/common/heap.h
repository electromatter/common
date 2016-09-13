#ifndef COMMON_HEAP_H
#define COMMON_HEAP_H

#include <common/vec.h>

#define OHEAP_ITEM(name)				\
	struct name {					\
		size_t index;				\
	}

#define OHEAP(name, type)	VEC(name, type*)

#define HEAP(name, type)	VEC(name, type)
#define HEAP_INITIALIZER	VEC_INITIALIZER
#define HEAP_INIT(heap)		VEC_INIT(heap)
#define HEAP_DESTROY(heap)	VEC_DESTROY(heap)
#define HEAP_EMPTY(heap)	VEC_EMPTY(heap)
#define HEAP_HEAD(heap)		VEC_AT(heap, 0)

/* test(parent, child)
 * returns non-zero if heap property is satisfied
 * parent <= child for a min heap
 * parent >= child for a max heap */
#define HEAP_GEN(attr, prefix, heap_type, type, test)	\
attr							\
prefix##init(heap_type *heap) {				\
	VEC_INIT(heap);					\
}							\
attr 							\
prefix##destroy(heap_type *heap) {			\
	VEC_DESTROY(heap);				\
}							\
attr void						\
prefix##compact(heap_type *heap, size_t new_capacity) {	\
	return VEC_EXPAND(heap, new_capacity, type);	\
}							\
attr void						\
prefix##compact(heap_type *heap) {			\
	VEC_COMPACT(heap);				\
}							\
attr void						\
prefix##siftup(heap_type *heap, size_t index) {		\
	size_t parent;					\
	type temp;					\
							\
	while (index > 0) {				\
		parent = (index - 1) / 2;		\
							\
		/* check if the heap is already valid */\
		if (test(heap->items[parent],		\
				heap->items[index]))	\
			break;				\
							\
		/* repair the heap and continue up. */	\
		temp = heap->items[parent];		\
		heap->items[parent]= heap->items[index];\
		heap->items[index] = temp;		\
		index = parent;				\
	}						\
}							\
attr void						\
prefix##siftdown(heap_type *heap, size_t index) {	\
	size_t child;					\
	type temp;					\
							\
	while (1) {					\
		child = 2 * index + 1;			\
		if (child > VEC_COUNT(heap))		\
			break;				\
							\
		if (child + 1 < VEC_COUNT(heap))	\
			if (!test(heap->items[child],	\
				heap->items[child + 1]))\
				child += 1;		\
							\
		/* check if heap is already valid */	\
		if (test(heap->items[item],		\
				heap->items[child]))	\
			break;				\
							\
		/* reapair the heap and continue. */	\
		temp = heap->items[child];		\
		heap->items[child]= heap->items[index];	\
		heap->items[index] = temp;		\
		index = child;				\
	}						\
}							\
attr int						\
prefix##push(heap_type *heap, type value) {		\
	size_t index;					\
	if (prefix##expand(heap) < 0)			\
		return -1;				\
	index = VEC_COUNT(heap)++;			\
	heap->items[index] = value;			\
	prefix##siftup(heap, index);			\
	return 0;					\
}							\
attr void						\
prefix##remove(heap_type *heap, size_t index) {		\
	size_t last = VEC_COUNT(heap) - 1;		\
	type temp;					\
							\
	if (index >= VEC_COUNT(heap))			\
		abort();				\
							\
	if (index != last) {				\
		temp = heap->items[last];		\
		heap->items[last]= heap->items[index];	\
		heap->items[index] = temp;		\
		prefix##siftdown(heap, index);		\
	}						\
							\
	VEC_COUNT(heap) -= 1;				\
}							\
attr value						\
prefix##pop(heap_type *heap) {				\
	if (HEAP_EMPTY(heap))				\
		abort();				\
	return prefix##remove(heap, VEC_COUNT(heap) - 1);\
}

/* test(parent, child)
 * returns non-zero if heap property is satisfied
 * parent <= child for a min heap
 * parent >= child for a max heap */
#define OHEAP_GEN(attr, prefix, heap_type, type, field, test)	\
attr								\
prefix##init(heap_type *heap) {					\
	VEC_INIT(heap);						\
}								\
attr 								\
prefix##destroy(heap_type *heap) {				\
	VEC_DESTROY(heap);					\
}								\
attr void							\
prefix##compact(heap_type *heap, size_t new_capacity) {		\
	return VEC_EXPAND(heap, new_capacity, type*);		\
}								\
attr void							\
prefix##compact(heap_type *heap) {				\
	VEC_COMPACT(heap);					\
}								\
attr void							\
prefix##siftup(heap_type *heap, type *item) {			\
	size_t index = item->field.index;			\
	size_t parent;						\
	type *temp;						\
								\
	while (index > 0) {					\
		parent = (index - 1) / 2;			\
								\
		/* check if the heap is already valid */	\
		if (test(heap->items[parent],			\
				heap->items[index]))		\
			break;					\
								\
		/* repair the heap and continue up. */		\
		temp = heap->items[parent];			\
		heap->items[parent]= heap->items[index];	\
		heap->items[index] = temp;			\
		heap->items[parent]->field.index = parent;	\
		heap->items[index]->field.index = index;	\
		index = parent;					\
	}							\
}								\
attr void							\
prefix##siftdown(heap_type *heap, type *item) {		\
	size_t index = item->field.index;			\
	size_t child;						\
	type *temp;						\
								\
	while (1) {						\
		child = 2 * index + 1;				\
		if (child > VEC_COUNT(heap))			\
			break;					\
								\
		if (child + 1 < VEC_COUNT(heap))		\
			if (!test(heap->items[child],		\
				heap->items[child + 1]))	\
				child += 1;			\
								\
		/* check if heap is already valid */		\
		if (test(heap->items[item],			\
				heap->items[child]))		\
			break;					\
								\
		/* reapair the heap and continue. */		\
		temp = heap->items[child];			\
		heap->items[child]= heap->items[index];		\
		heap->items[index] = temp;			\
		index = child;					\
	}							\
}								\
attr int							\
prefix##push(heap_type *heap, type *value) {			\
	if (prefix##expand(heap) < 0)				\
		return -1;					\
	value->field.index = VEC_COUNT(heap)++;			\
	heap->items[value->field.index] = value;		\
	prefix##siftup(heap, index);				\
	return 0;						\
}								\
attr void							\
prefix##remove(heap_type *heap, type *item) {			\
	size_t index = item->field.index;			\
	size_t last = VEC_COUNT(heap) - 1;			\
	type *temp;						\
								\
	if (index >= VEC_COUNT(heap))				\
		abort();					\
								\
	if (index != last) {					\
		temp = heap->items[last];			\
		heap->items[last]= heap->items[index];		\
		heap->items[index] = temp;			\
		prefix##siftdown(heap, heap->items[index]);	\
	}							\
								\
	VEC_COUNT(heap) -= 1;					\
}								\
attr value							\
prefix##pop(heap_type *heap) {					\
	if (HEAP_EMPTY(heap))					\
		abort();					\
	return prefix##remove(heap, VEC_COUNT(heap) - 1);	\
}

#endif

