#ifndef TABLE_H
#define TABLE_H

#include <stdlib.h>
#include <stdint.h>

#include <common/hash.h>

/* Call table_update if entry's hash code possibly changes to reinsert
 * into table. */
struct table_entry
{
	struct table_entry *next;
	uint64_t hash;
};

struct table
{
	size_t size, load;
	struct table_entry **table;
};

/* Returns zero on equality, non-zero otherwise. */
typedef int table_cmp_func(const struct table_entry *a, const struct table_entry *b);
/* Returns the hash value of the given entry. Two entries that may
 * compare equal MUST have the same hash value. Collisions are allowed. */
typedef uint64_t table_hash_func(const struct table_entry *ent);

/* Resizes the table to have size buckets. */
int table_resize(struct table *tab, size_t size);

/* Initializes a table with size buckets. */
static inline
int table_init(struct table *tab, size_t size)
{
	tab->size = 0;
	tab->load = 0;
	tab->table = NULL;
	return table_resize(tab, size);
}

/* Frees memory associated with the table. Warning: if the table
 * was not empty, then the entries may be leaked. */
static inline
void table_destroy(struct table *tab)
{
	tab->size = 0;
	tab->load = 0;
	free(tab->table);
}

/* Returns the next entry that is equal to ent in the table that ent is
 * a part of. NOTE: if ent is removed, next will return NULL.
 * ent MUST still be valid. */
static inline
struct table_entry *table_next(struct table_entry *ent, table_cmp_func cmp)
{
	struct table_entry *next;

	if (ent == NULL)
		return NULL;

	next = ent->next;
	while (next != NULL && cmp(ent, next))
		next = next->next;

	return next;
}

/* Returns the first entry that is equal to key. */
static inline
struct table_entry *table_first(struct table *tab, struct table_entry *key,
		table_hash_func hash, table_cmp_func cmp)
{
	struct table_entry *ent;

	if (tab->size == 0)
		return NULL;

	ent = tab->table[hash(key) % tab->size];
	while (ent != NULL && cmp(ent, key))
		ent = ent->next;

	return ent;
}

/* Inserts ent into the table. Duplicates are allowed, so long
 * as the same ent is not re-inserted. */
static inline
void table_insert(struct table *tab, struct table_entry *ent,
		table_hash_func hash)
{
	if (tab->size == 0)
		abort();

	ent->hash = hash(ent);

#ifndef NDEBUG
	struct table_entry *prev = tab->table[ent->hash % tab->size];
	while (prev != NULL) {
		if (prev == ent)
			abort();
		prev = prev->next;
	}
#endif

	ent->next = tab->table[ent->hash % tab->size];
	tab->table[ent->hash % tab->size] = ent;
	tab->load += 1;
}

/* Removes a given ent from a table. If ent is not in the table, this
 * is harmless. */
void table_remove(struct table *tab, struct table_entry *ent);

/* Fused table_first and table_remove. Finds the first entry that is equal
 * to key, removes it, and returns it. */
static inline
struct table_entry *table_pop(struct table *tab, struct table_entry *key,
		table_hash_func hash, table_cmp_func cmp)
{
	struct table_entry *ent, **prev;

	if (tab->size == 0)
		return NULL;

	prev = &tab->table[hash(key) % tab->size];
	ent = *prev;
	while (ent != NULL && cmp(ent, key))
		prev = &ent->next, ent = ent->next;

	if (ent == NULL)
		return NULL;

	*prev = ent->next;
	tab->load -= 1;
	return ent;
}

/* Call when ent's hash value possibly changes. Reassignes ent to
 * the correct bucket. */
static inline
void table_update(struct table *tab, struct table_entry *ent,
		table_hash_func hash)
{
	if (tab->size == 0 || ent->hash % tab->size == hash(ent) % tab->size)
		return;
	table_remove(tab, ent);
	table_insert(tab, ent, hash);
}

#endif

