#ifndef COMMON_VEC_H
#define COMMON_VEC_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*for ssize_t*/
#include <sys/types.h>

#define VEC_INITIAL_SIZE	(8)

#define FLAT_VEC(name, type)			\
struct name {					\
	size_t num_items, max_items;		\
	type *items;				\
}

#define VEC_INITIALIZER				\
	{0, 0, NULL}

#define VEC_COUNT(vec)		((vec)->num_items)
#define VEC_SIZE(vec, type)	(VEC_COUNT(vec) * sizeof(type))
#define VEC_CAPACITY(vec)	((vec)->max_items)
#define VEC_EMPTY(vec)		(VEC_COUNT(vec) == 0)
#define VEC_AT(vec, index)	((vec)->items[(index)])
#define VEC_ITEMS(vec)		((vec)->items)

#define VEC_GEN(attr, prefix, vec_type, type)				\
attr int								\
prefix##resize(vec_type *vec, size_t new_capacity) {			\
	type *items;							\
									\
	if (new_capacity == VEC_CAPACITY(vec))				\
		return 0;						\
									\
	if (new_capacity < VEC_COUNT(vec))				\
		return -1;						\
									\
	items = realloc((vec)->items, new_capacity * sizeof(type));	\
	if (items == NULL)						\
		return -1;						\
									\
	VEC_CAPACITY(vec) = new_capacity;				\
	(vec)->items = items;						\
	return 0;							\
}									\
attr int								\
prefix##expand(vec_type *vec, size_t extra_capacity) {			\
	size_t new_capacity;						\
	if (extra_capacity > SIZE_MAX / 2 - VEC_COUNT(vec))		\
		return -1;						\
									\
	if (VEC_COUNT(vec) + extra_capacity <= VEC_CAPACITY(vec))	\
		return 0;						\
									\
	new_capacity = VEC_CAPACITY(vec);				\
	if (new_capacity == 0)						\
		new_capacity = VEC_INITIAL_SIZE;			\
									\
	while (new_capacity < VEC_COUNT(vec) + extra_capacity)		\
		new_capacity *= 2;					\
									\
	return prefix##resize(vec, new_capacity);			\
}									\
attr void								\
prefix##compact(vec_type *vec) {					\
	prefix##resize(vec, VEC_COUNT(vec));				\
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
	if (prefix##expand(vec, count) < 0)				\
		return -1;						\
	memmove(vec->items + count, vec->items, VEC_SIZE(vec, type));	\
	return 0;							\
}									\
attr int								\
prefix##truncate(vec_type *vec, size_t count) {				\
	if (count > VEC_COUNT(vec))					\
		if (prefix##expand(vec, count - VEC_COUNT(vec)) < 0)	\
			return -1;					\
	VEC_COUNT(vec) = count;						\
	return 0;							\
}									\
attr int								\
prefix##insert(vec_type *vec, size_t index, type value) {		\
	size_t i;							\
	if (prefix##expand(vec, 1) < 0)					\
		return -1;						\
	for (i = VEC_COUNT(vec) + 1; i > index; i--)			\
		VEC_AT(vec, i) = VEC_AT(vec, i - 1);			\
	VEC_AT(vec, index) =  value;					\
	VEC_COUNT(vec) += 1;						\
	return 0;							\
}									\
attr void								\
prefix##remove(vec_type *vec, size_t index) {				\
	if (index >= VEC_COUNT(vec))					\
		return;							\
	for (index += 1; index < VEC_COUNT(vec); index++)		\
		VEC_AT(vec, index - 1) = VEC_AT(vec, index);		\
	VEC_COUNT(vec) -= 1;						\
}									\
attr int								\
prefix##push(vec_type *vec, type value) {				\
	if (prefix##expand(vec, 1) < 0)					\
		return -1;						\
	VEC_AT(vec, VEC_COUNT(vec)) = value;				\
	VEC_COUNT(vec) += 1;						\
	return 0;							\
}									\
attr int								\
prefix##peek(vec_type *vec, type *value) {				\
	if (VEC_EMPTY(vec))						\
		return -1;						\
	if (value != NULL)						\
		*value = VEC_AT(vec, 0);				\
	return 0;							\
}									\
attr int								\
prefix##pop(vec_type *vec, type *value) {				\
	if (prefix##peek(vec, value) < 0)				\
		return -1;						\
	prefix##remove(vec, 0);						\
	return 0;							\
}									\
attr void								\
prefix##swap(vec_type *vec, size_t i, size_t j) {			\
	if (i == j)							\
		return;							\
	type temp = VEC_AT(vec, i);					\
	VEC_AT(vec, i) = VEC_AT(vec, j);				\
	VEC_AT(vec, j) = temp;						\
}									\
attr void								\
prefix##reverse(vec_type *vec) {					\
	ssize_t i = 0, j = VEC_COUNT(vec) - 1;				\
	while (i < j)							\
		prefix##swap(vec, i++, j--);				\
}									\
attr void								\
prefix##init(vec_type *vec) {						\
	(vec)->num_items = 0;						\
	(vec)->max_items = 0;						\
	(vec)->items = NULL;						\
}									\
attr void								\
prefix##destroy(vec_type *vec) {					\
	(vec)->num_items = 0;						\
	(vec)->max_items = 0;						\
	free((vec)->items);						\
	(vec)->items = NULL;						\
}									\
attr int								\
prefix##append(vec_type *vec, type *items, size_t count) {		\
	if (prefix##expand(vec, count) < 0)				\
		return -1;						\
	memcpy(vec->items+ vec->num_items, items, count * sizeof(type));\
	VEC_COUNT(vec) += count;					\
	return 0;							\
}									\
attr int								\
prefix##prepend(vec_type *vec, type *items, size_t count) {		\
	if (prefix##shift(vec, count) < 0)				\
		return -1;						\
	memcpy(vec->items, items, count * sizeof(type));		\
	return 0;							\
}

#define VEC_PROTOTYPE(attr, prefix, vec_type, type)			\
attr int								\
prefix##resize(vec_type *vec, size_t new_capacity);			\
attr int								\
prefix##expand(vec_type *vec, size_t extra_capacity);			\
attr void								\
prefix##compact(vec_type *vec);						\
attr void								\
prefix##skip(vec_type *vec, size_t count);				\
attr int								\
prefix##shift(vec_type *vec, size_t count);				\
attr int								\
prefix##truncate(vec_type *vec, size_t count);				\
attr int								\
prefix##insert(vec_type *vec, size_t index, type value);		\
attr void								\
prefix##remove(vec_type *vec, size_t index);				\
attr int								\
prefix##push(vec_type *vec, type value);				\
attr int								\
prefix##peek(vec_type *vec, type *value);				\
attr int								\
prefix##pop(vec_type *vec, type *value);				\
attr void								\
prefix##swap(vec_type *vec, size_t i, size_t j);			\
attr void								\
prefix##reverse(vec_type *vec);						\
attr void								\
prefix##init(vec_type *vec);						\
attr void								\
prefix##destroy(vec_type *vec);						\
attr int								\
prefix##append(vec_type *vec, type *items, size_t count);		\
attr int								\
prefix##prepend(vec_type *vec, type *items, size_t count);

#endif

