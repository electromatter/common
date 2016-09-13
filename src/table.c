#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "table.h"

#define DEFAULT_TABLE_SIZE	(64)

int table_resize(struct table *tab, size_t size)
{
	size_t prime, i, load;
	struct table_entry **new_table, *next, *cur;

	if (size == 0)
		size = DEFAULT_TABLE_SIZE;

	prime = pow2_prime(size);
	if (prime < size)
		return 1;

	new_table = malloc(prime * sizeof(*new_table));
	if (new_table == NULL)
		return 1;
	memset(new_table, 0, prime * sizeof(*new_table));

	load = 0;
	for (i = 0; i < tab->size; i++) {
		next = tab->table[i];
		while ((cur = next) != NULL) {
			next = cur->next;
			cur->next = new_table[cur->hash % prime];
			new_table[cur->hash % prime] = cur;
			load += 1;
		}
	}

	free(tab->table);
	tab->size = prime;
	tab->load = load;
	tab->table = new_table;
	return 0;
}

void table_recount(struct table *tab)
{
	size_t i, load;
	struct table_entry *cur;

	load = 0;
	for (i = 0; i < tab->size; i++) {
		cur = tab->table[i];
		while (cur != NULL) {
			cur = cur->next;
			load += 1;
		}
	}

	tab->load = load;
}

void table_remove(struct table *tab, struct table_entry *ent)
{
	struct table_entry *cur, **prev;

	if (tab->size == 0)
		return;

	prev = &tab->table[ent->hash % tab->size];
	cur = *prev;

	while (cur != NULL && cur != ent)
		prev = &cur->next, cur = cur->next;

	if (cur == NULL)
		return;

	*prev = ent->next;
	ent->next = NULL;
	tab->load -= 1;
}

