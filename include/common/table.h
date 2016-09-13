#ifndef COMMON_TABLE_H
#define COMMON_TABLE_H

#include <stdlib.h>

#define TABLE(name, type)						\
struct name {								\
	size_t num_buckets, load;					\
	type **buckets;							\
}

#define TABLE_INITIALIZER						\
	{0, 0, NULL}

#define TABLE_ENTRY(type, hash_type)					\
struct {								\
	type *next;							\
	hash_type code;							\
}

#define TABLE_ENTRY_INITIALIZER						\
	{NULL, 0}

#define TABLE_NUM_BUCKETS(table)	((table)->num_buckets)
#define TABLE_LOAD(table)		((table)->load)

#define TABLE_BUCKET(table, code)					\
	((table)->buckets[(code) % (table)->num_buckets])

#define TABLE_NEXT(table, elm, field)					\
	((elm) == NULL ? NULL : ((elm)->field.next != NULL ?		\
		(elm)->field.next) : TABLE_BUCKET(table, elm->field.code + 1))

#define TABLE_FIRST(table)						\
	((table)->num_buckets == 0 ? NULL : (table)->buckets[0])

/* UNSAFE ACROSS INSERT AND REMOVE */
#define TABLE_FOREACH(var, table, field)				\
	for (var = TABLE_FIRST(table);					\
		var != NULL;						\
		var = TABLE_NEXT(table, var, field))

/* UNSAFE ACROSS TABLE_RESIZE */
#define TABLE_FOREACH_SAFE(var, table, field, next)			\
	for (var = TABLE_FIRST(table);					\
		var != NULL						\
		&& (next = TABLE_NEXT(table, var, field), 1);		\
		var = next)

#define TABLE_INIT(table)	do {					\
		(table)->num_buckets = 0;				\
		(table)->load = 0;					\
		(table)->buckets = NULL;				\
} while (0)

#define TABLE_DESTROY(table)	do { 					\
	if ((table) == NULL)						\
		break;							\
	(table)->num_buckets = 0;					\
	(table)->load = 0;						\
	free((table)->buckets);						\
	(table)->buckets = NULL;					\
} while (0)

/* cmp(a, b) returns non-zero if a, b are not equal
 * hash(a) returns the hash code of a, if cmp(a, b) could return zero,
 * then hash(a) = hash(b)! */
#define TABLE_GEN(attr, preifx, table_type, type, field, hash, cmp)	\
attr type *								\
preifx##next_equal(table_type *table, type *elm) {			\
	type *key = elm;						\
	if (elm == NULL)						\
		return NULL;						\
	elm = elm->field.next;						\
	while (elm != NULL && cmp(key, elm))				\
		next = elm->field.next;					\
	return elm;							\
}									\
attr type *								\
prefix##first_equal(table_type *table, type *key) {			\
	type *next;							\
	if (TABLE_NUM_BUCKETS(table) == 0)				\
		return NULL;						\
	next = TABLE_BUCKET(table, hash(key));				\
	while (next != NULL && cmp(key, next))				\
		next = next->field.next;				\
	return next;							\
}									\
attr void								\
prefix##push(table_type *table, type *elm) {				\
	if (TABLE_NUM_BUCKETS(table) == 0)				\
		abort();						\
	elm->field.code = hash(elm);					\
	elm->field.next = TABLE_BUCKET(table, elm->field.code);		\
	TABLE_BUCKET(table, elm->field.code) = elm;			\
	TABLE_LOAD(table) += 1;						\
}									\
attr void								\
preifx##remove(table_type *table, type *elm) {				\
	type *prev;							\
	if (TABLE_NUM_BUCKETS(table) == 0)				\
		return;							\
	elm->field.code = hash(elm);					\
	elm->field.next = TABLE_BUCKET(table, elm->field.code);		\
	TABLE_BUCKET(table, elm->field.code) = elm;			\
}									\
attr int								\
prefix##resize(table_type *table, size_t num_buckets) {			\
	size_t i;							\
	type **old_table = table->buckets, **new_table = NULL,		\
		*next, *cur;						\
									\
	if (num_buckets == 0)						\
		goto done;						\
									\
	new_table = calloc(num_buckets, sizeof(void*));			\
	if (new_table == NULL)						\
		return -1;						\
									\
	for (i = 0; i < TABLE_NUM_BUCKETS(table); i++) {		\
		cur = old_table[i];					\
		while (cur != NULL) {					\
			next = cur->field.next;				\
			cur->field.next =				\
				new_table[				\
					cur->field.code % num_buckets];	\
			new_table[cur->field.code % num_buckets] = cur;	\
			cur = next;					\
		}							\
	}								\
									\
done:									\
	free(old_table);						\
	table->buckets = new_table;					\
	table->num_buckets = num_buckets;				\
	return 0;							\
}									\
attr void								\
preifx##init(table_type *table, size_t size) {				\
	TABLE_INIT(table);						\
	prefix##resize(table, size);					\
}									\
attr void								\
prefix##destroy(table_type *table) {					\
	TABLE_DESTROY(table);						\
}									\
attr type *								\
preifx##pop(table_type *type, type *key) {				\
	type *elm = prefix##first_equal(table, type *key);		\
	preifx##remove(table, elm);					\
	return elm;							\
}									\
attr type *								\
prefix##replace_first(table_type *table, type *elm) {			\
	type *old = prefix##pop(table, elm);				\
	preifx##push(table, elm);					\
	return old;							\
}									\
attr void								\
prefix##update(table_type *table, type *elm) {				\
	prefix##remove(table, elm);					\
	prefix##push(table, elm);					\
}

#endif

