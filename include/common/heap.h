#ifndef COMMON_HEAP_H
#define COMMON_HEAP_H

#include <common/vec.h>

#define VEC_HEAP(name, type)		FLAT_VEC(name, type)
#define HEAP_INITIALIZER		VEC_INITIALIZER
#define HEAP_EMPTY(heap)		VEC_EMPTY(heap)

#define HEAP_UPDATE_NOP(item, new_index)

/* test(parent, child)
 * must return non-zero if the heap property is satisfied,
 * and zero if it is violated.
 * 
 * for min-heaps: parent <= child
 * and max-heaps: parent >= child
 *
 * update_index(item, new_index)
 * if the type contained in the heap is an object, and the
 * object has a field that tracks the index in the heap,
 * update_index must set the index on that object.
 */
#define BASE_HEAP_GEN(attr, prefix, heap_type, type, test, update_index)\
VEC_GEN(attr, prefix##_vec_, heap_type, type)				\
attr void								\
prefix##init(heap_type *heap) {						\
	prefix##_vec_init(heap);					\
}									\
attr void								\
prefix##destroy(heap_type *heap) {					\
	prefix##_vec_destroy(heap);					\
}									\
attr void								\
prefix##siftup(heap_type *heap, size_t index) {				\
	size_t parent;							\
	if (index >= VEC_COUNT(heap))					\
		return;							\
	while (index > 0) {						\
		parent = (index - 1) / 2;				\
		if (test(VEC_AT(heap, parent), VEC_AT(heap, index)))	\
			break;						\
		prefix##_vec_swap(heap, parent, index);			\
		update_index(VEC_AT(heap, parent), parent);		\
		update_index(VEC_AT(heap, index), index);		\
		index = parent;						\
	}								\
}									\
attr void								\
prefix##siftdown(heap_type *heap, size_t index) {			\
	size_t child;							\
	while (1) {							\
		child = 2 * index + 1;					\
		if (child > VEC_COUNT(heap))				\
			break;						\
		if (child + 1 < VEC_COUNT(heap))			\
			if (!test(VEC_AT(heap, child),			\
					VEC_AT(heap, child + 1)))	\
				child += 1;				\
		if (test(VEC_AT(heap, index), VEC_AT(heap, child)))	\
			break;						\
		prefix##_vec_swap(heap, index, child);			\
		update_index(VEC_AT(heap, child), child);		\
		update_index(VEC_AT(heap, index), index);		\
		index = child;						\
	}								\
}									\
attr void								\
prefix##update(heap_type *heap, size_t index) {				\
	prefix##siftup(heap, index);					\
	prefix##siftdown(heap, index);					\
}									\
attr int								\
prefix##peek(heap_type *heap, type *value) {				\
	return prefix##_vec_peek(heap, value);				\
}									\
attr int								\
prefix##push(heap_type *heap, type value) {				\
	if (prefix##_vec_push(heap, value) < 0)				\
		return -1;						\
	update_index(VEC_AT(heap, VEC_COUNT(heap) - 1),			\
			VEC_COUNT(heap) - 1);				\
	prefix##siftup(heap, VEC_COUNT(heap) - 1);			\
	return 0;							\
}									\
attr void								\
prefix##remove(heap_type *heap, size_t index) {				\
	size_t tail = VEC_COUNT(heap) - 1;				\
	if (VEC_EMPTY(heap))						\
		return;							\
	if (tail != index)						\
		prefix##_vec_swap(heap, index, tail);			\
	VEC_COUNT(heap) -= 1;						\
	prefix##update(heap, index);					\
}									\
attr int								\
prefix##pop(heap_type *heap, type *value) {				\
	if (prefix##peek(heap, value) < 0)				\
		return -1;						\
	prefix##remove(heap, 0);					\
	return 0;							\
}									\
attr void								\
prefix##heapify(heap_type *heap) {					\
	size_t i;							\
	for (i = 0; i < VEC_COUNT(heap); i++)				\
		prefix##siftup(heap, i);				\
}

#define BASE_HEAP_PROTOTYPE(attr, prefix, heap_type, type)		\
VEC_GEN(attr, prefix##_vec_, heap_type, type)				\
attr void								\
prefix##init(heap_type *heap);						\
attr void								\
prefix##destroy(heap_type *heap);					\
attr void								\
prefix##siftup(heap_type *heap, size_t index);				\
attr void								\
prefix##siftdown(heap_type *heap, size_t index);			\
attr void								\
prefix##update(heap_type *heap, size_t index);				\
attr int								\
prefix##peek(heap_type *heap, type *value);				\
attr int								\
prefix##push(heap_type *heap, type value);				\
attr void								\
prefix##remove(heap_type *heap, size_t index);				\
attr int								\
prefix##pop(heap_type *heap, type *value);				\
attr void								\
prefix##heapify(heap_type *heap);					\

#define HEAP_SYMBOLS(prefix, symbol)					\
VEC_SYMBOLS(prefix##_vec_, symbol)					\
symbol(prefix##init)							\
symbol(prefix##destroy)							\
symbol(prefix##siftup)							\
symbol(prefix##siftdown)						\
symbol(prefix##update)							\
symbol(prefix##peek)							\
symbol(prefix##push)							\
symbol(prefix##remove)							\
symbol(prefix##pop)							\
symbol(prefix##heapify)

#define OHEAP_GEN(attr, prefix, heap_type, type, test, update_index)	\
	BASE_HEAP_GEN(attr, prefix, heap_type, type*, test, update_index)

#define OHEAP_PROTOTYPE(attr, prefix, heap_type, type)			\
	BASE_HEAP_PROTOTYPE(attr, prefix, heap_type, type*)

#define HEAP_GEN(attr, prefix, heap_type, type, test)			\
	BASE_HEAP_GEN(attr, prefix, heap_type, type, test, HEAP_UPDATE_NOP)

#define HEAP_PROTOTYPE(attr, prefix, heap_type, type)			\
	BASE_HEAP_PROTOTYPE(attr, prefix, heap_type, type)

#endif

