#ifndef COMMON_VEC_H
#define COMMON_VEC_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define VEC_INITIAL_SIZE	(8)

#define VEC(name, type)				\
struct name {					\
	size_t num_items, max_items;		\
	type *items;				\
}

#define VEC_INITIALIZER				\
	{0, 0, NULL}

#define VEC_INIT(vec)	do {			\
		(vec)->num_items = 0;		\
		(vec)->max_items = 0;		\
		(vec)->items = NULL;		\
	}
#define VEC_DESTROY(vec)	do {		\
		(vec)->num_items = 0;		\
		(vec)->max_items = 0;		\
		free((vec)->items);		\
		(vec)->items = NULL;		\
	}

#define VEC_COUNT(vec)		((vec)->num_items)
#define VEC_CAPACITY(vec)	((vec)->max_items)
#define VEC_EMPTY(vec)		(VEC_COUNT(vec) == 0)
#define VEC_AT(vec, index)	((vec)->items[(index)])

#define VEC_EXPAND(vec, size, type) (do {				\
	size_t new_size = VEC_CAPACITY(vec);				\
	type *items;							\
									\
	if (new_size >= size)						\
		break;							\
									\
	if (new_size == 0)						\
		new_size = VEC_INITIAL_SIZE;				\
									\
	while (new_size < size)						\
		new_size *= 2;						\
									\
	items = realloc((vec)->items, new_size * sizeof(type));		\
	if (items == NULL)						\
		break;							\
									\
	VEC_CAPACITY(vec) = new_size;					\
	(vec)->items = items;						\
} while(0), (VEC_CAPACITY(vec) < new_capacity ? -1 : 0))

#define VEC_COMPACT(vec, type) do {					\
	VEC_CAPACITY(vec) = VEC_COUNT(vec);				\
	type *items;							\
									\
	if (VEC_CAPACITY(vec) == 0) {					\
		free(vec->items)					\
		vec->items = NULL;					\
		return;							\
	}								\
									\
	items = realloc(vec->item, VEC_CAPACITY(vec) * sizeof(type));	\
	if (items == NULL)						\
		return;							\
									\
	vec->items = items;						\
} while (0)

#define VEC_GEN(attr, prefix, vec_type, type)				\
attr int								\
prefix##expand(vec_type *vec, size_t new_capacity) {			\
	return VEC_EXPAND(vec, type);					\
}									\
attr void								\
prefix##compact(vec_type *vec) {					\
	VEC_COMPACT(vec, type);						\
}									\
attr void								\
prefix##skip(vec_type *vec, size_t count) {				\
	size_t size = (VEC_COUNT(vec) - count) * sizeof(type);		\
	if (count >= VEC_COUNT(vec)) {					\
		VEC_COUNT(vec) = 0;					\
		return;							\
	}								\
	memmove(vec->items, vec->items + count, size);			\
	VEC_COUNT(vec) -= count;					\
}									\
attr int								\
prefix##shift(vec_type *vec, size_t count) {				\
	size_t size = VEC_COUNT(vec) * sizeof(type);			\
	if (prefix##expand(vec, count + VEC_COUNT(vec)) < 0)		\
		return -1;						\
	memove(vec->items + count, vec->items, size);			\
	return 0;							\
}									\
attr int								\
prefix##truncate(vec_type *vec, size_t count) {				\
	if (prefix##expand(vec, count) < 0)				\
		return -1;						\
	VEC_COUNT(vec) = count;						\
}									\
attr void								\
prefix##insert(vec_type *vec, size_t index, type value) {		\
	size_t i;							\
	if (prefix##expand(vec, VEC_COUNT(vec) + 1) < 0)		\
		return -1;						\
	for (i = VEC_COUNT(vec) + 1; i > index; i--)			\
		VEC_AT(vec, i) = VEC_AT(vec, i - 1);			\
	VEC_AT(vec, index) =  value;					\
	VEC_COUNT(vec) += 1;						\
}									\
attr void								\
prefix##remove(vec_type *vec, size_t index) {				\
	if (index >= VEC_COUNT(vec))					\
		abort();						\
	for (index += 1; index < VEC_COUNT(vec); index++)		\
		VEC_AT(vec, index - 1) = VEC_AT(vec, index);		\
	VEC_COUNT(vec) -= 1;						\
}									\
attr void								\
prefix##swap(vec_type *vec, size_t i, size_t j) {			\
	type temp = VEC_AT(vec, i);					\
	VEC_AT(vec, i) = VEC_AT(vec, j);				\
	VEC_AT(vec, j) = temp;						\
}									\
attr void								\
prefix##reverse(vec_type *vec) {					\
	ssize_t i = 0, j = VEC_COUNT(vec) - 1;				\
	while (i < j)							\
		prefix##swap(i++, j--);					\
}									\
attr void								\
prefix##init(vec_type *vec) {						\
	VEC_INIT(vec);							\
}									\
attr void								\
preifx##destroy(vec_type *vec) {					\
	VEC_DESTROY(vec);						\
}

#endif
