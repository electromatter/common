/*TODO base on vec.h, VEC_TABLE*/
#ifndef COMMON_TABLE_H
#define COMMON_TABLE_H

#include <stdlib.h>

#include <common/hash.h>

#define LINKED_TABLE(name, type)					\
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

#define TABLE_BUCKET(table, code)					\
	((table)->buckets[(code) % (table)->num_buckets])

#define TABLE_INITIAL_SIZE	(16)

/* cmp(a, b) returns non-zero if a, b are not equal
 * hash(a) returns the hash code of a, if cmp(a, b) could return zero,
 * then hash(a) = hash(b)! */
#define TABLE_GEN(attr, prefix, table_type, type, field, hash, cmp)	\
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
	for (i = 0; i < table->num_buckets; i++) {			\
		cur = old_table[i];					\
		while (cur != NULL) {					\
			next = cur->field.next;				\
			cur->field.next =				\
			  new_table[cur->field.code % num_buckets];	\
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
attr int								\
prefix##expand(table_type *table, size_t extra) {			\
	size_t prime;							\
	if (table->load > SIZE_MAX - extra)				\
		return -1;						\
	if (table->load + extra < table->num_buckets)			\
		return 0;						\
	prime = next_pow2_prime(table->load + extra);			\
	if (prime < table->load + extra)				\
		return -1;						\
	return prefix##resize(table, prime);				\
}									\
attr void								\
prefix##contract(table_type *table) {					\
	size_t prime = pow2_prime(table->load);				\
	if (prime < table->load)					\
		return;							\
	prefix##resize(table, prime);					\
}									\
attr void								\
prefix##init(table_type *table) {					\
	table->num_buckets = 0;						\
	table->load = 0;						\
	table->buckets = NULL;						\
}									\
attr void								\
prefix##destroy(table_type *table) {					\
	if (table == NULL)						\
		return;							\
	table->num_buckets = 0;						\
	table->load = 0;						\
	free(table->buckets);						\
	table->buckets = NULL;						\
}									\
attr type *								\
prefix##first_equal(table_type *table, type *key) {			\
	type *next;							\
	if (table == NULL)						\
		return NULL;						\
	if (table == NULL || table->num_buckets == 0)			\
		return NULL;						\
	next = TABLE_BUCKET(table, hash(key));				\
	while (next != NULL && cmp(next, key))				\
		next = next->field.next;				\
	return next;							\
}									\
attr type *								\
prefix##next_equal(type *elm) {						\
	type *key = elm;						\
	if (elm == NULL)						\
		return NULL;						\
	elm = elm->field.next;						\
	while (elm != NULL && cmp(elm, key))				\
		elm = elm->field.next;					\
	return elm;							\
}									\
attr type *								\
prefix##first(table_type *table) {					\
	size_t bucket;							\
	if (table == NULL)						\
		return NULL;						\
	for (bucket = 0; bucket < table->num_buckets; bucket++)		\
		if (table->buckets[bucket] != NULL)			\
			return table->buckets[bucket];			\
	return NULL;							\
}									\
attr type *								\
prefix##next(table_type *table, type *elm) {				\
	size_t bucket;							\
	if (elm == NULL)						\
		return NULL;						\
	if (elm->field.next != NULL)					\
		return elm->field.next;					\
	if (table == NULL || table->num_buckets == 0)			\
		return NULL;						\
	bucket = elm->field.code % table->num_buckets;			\
	for (bucket++; bucket < table->num_buckets; bucket++)		\
		if (table->buckets[bucket] != NULL)			\
			return table->buckets[bucket];			\
	return NULL;							\
}									\
attr int								\
prefix##push(table_type *table, type *elm) {				\
	if (elm == NULL)						\
		return 0;						\
	if (table == NULL)						\
		return -1;						\
	if (prefix##expand(table, 1) < 0 && table->num_buckets == 0)	\
		return -1;						\
	elm->field.code = hash(elm);					\
	elm->field.next = TABLE_BUCKET(table, elm->field.code);		\
	TABLE_BUCKET(table, elm->field.code) = elm;			\
	table->load += 1;						\
	return 0;							\
}									\
attr void								\
prefix##remove(table_type *table, type *elm) {				\
	type **ptr;							\
	if (table == NULL || table->num_buckets == 0 || elm == NULL)	\
		return;							\
	ptr = &TABLE_BUCKET(table, elm->field.code);			\
	while (*ptr != elm) {						\
		if ((*ptr)->field.next == NULL)				\
			return;						\
		ptr = &(*ptr)->field.next;				\
	}								\
	*ptr = elm->field.next;						\
}									\
attr type *								\
prefix##pop(table_type *table, type *key) {				\
	type *elm = prefix##first_equal(table, key);			\
	prefix##remove(table, elm);					\
	return elm;							\
}									\
attr type *								\
prefix##replace_first(table_type *table, type *elm) {			\
	type *old = prefix##pop(table, elm);				\
	prefix##push(table, elm);					\
	return old;							\
}									\
attr int								\
prefix##update(table_type *table, type *elm) {				\
	prefix##remove(table, elm);					\
	return prefix##push(table, elm);				\
}

#define TABLE_PROTOTYPE(attr, prefix, table_type, type)			\
attr int								\
prefix##resize(table_type *table, size_t num_buckets);			\
attr int								\
prefix##expand(table_type *table, size_t extra);			\
attr void								\
prefix##contract(table_type *table);					\
attr void								\
prefix##init(table_type *table);					\
attr void								\
prefix##destroy(table_type *table);					\
attr type *								\
prefix##first_equal(table_type *table, type *key);			\
attr type *								\
prefix##next_equal(type *elm);						\
attr type *								\
prefix##first(table_type *table);					\
attr type *								\
prefix##next(table_type *table, type *elm);				\
attr int								\
prefix##push(table_type *table, type *elm);				\
attr void								\
prefix##remove(table_type *table, type *elm);				\
attr type *								\
prefix##pop(table_type *table, type *key);				\
attr type *								\
prefix##replace_first(table_type *table, type *elm);			\
attr int								\
prefix##update(table_type *table, type *elm);

#define TABLE_SYMBOLS(prefix, symbol)					\
symbol(prefix##resize)							\
symbol(prefix##expand)							\
symbol(prefix##contract)						\
symbol(prefix##init)							\
symbol(prefix##destroy)							\
symbol(prefix##first_equal)						\
symbol(prefix##next_equal)						\
symbol(prefix##first)							\
symbol(prefix##next)							\
symbol(prefix##push)							\
symbol(prefix##remove)							\
symbol(prefix##pop)							\
symbol(prefix##replace_first)						\
symbol(prefix##update)

#endif

