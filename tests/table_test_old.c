#include <stdlib.h>
#include <stdio.h>

#include <common/util.h>
#include <common/table.h>

union uuid
{
	unsigned char u8[16];
	uint64_t u64[2];
};

struct entity
{
	struct table_entry tab_ent;
	uint64_t id;
};

struct entity_table
{
	struct table tab;
};

static uint32_t murmur3_seed;

static uint64_t entity_hash(const struct table_entry *e)
{
	const struct entity *ent;
	uint32_t hash = murmur3_seed;
	ent = containerof(e, const struct entity, tab_ent);
	hash = murmur3_update(hash, ent->id);
	hash = murmur3_update(hash, ent->id >> 32);
	return murmur3_final(hash, 8);
}

static int entity_cmp(const struct table_entry *a, const struct table_entry *b)
{
	const struct entity *left, *right;
	left = containerof(a, const struct entity, tab_ent);
	right = containerof(b, const struct entity, tab_ent);
	return left->id == right->id ? 0 : 1;
}

struct entity *entity_get(struct entity_table *tab, uint64_t id)
{
	struct entity key;
	struct table_entry *entry;
	key.id = id;
	entry = table_first(&tab->tab, &key.tab_ent, entity_hash, entity_cmp);
	return containerof(entry, struct entity, tab_ent);
}

struct entity *entity_new(struct entity_table *tab, uint64_t id)
{
	struct entity *ent;

	if (entity_get(tab, id) != NULL)
		return NULL;
       
	ent = malloc(sizeof(*ent));
	if (ent == NULL)
		return NULL;

	ent->id = id;

	if (tab->tab.load > tab->tab.size) {
		table_resize(&tab->tab, tab->tab.size * 3);
		printf("GROW %li\n", tab->tab.size);
	}

	table_insert(&tab->tab, &ent->tab_ent, entity_hash);
	return ent;
}

void entity_free(struct entity_table *tab, uint64_t id)
{
	struct entity *ent = entity_get(tab, id);
	
	if (ent == NULL)
		return;

	table_remove(&tab->tab, &ent->tab_ent);
	free(ent);

	if (tab->tab.load < tab->tab.size / 2 && tab->tab.size > 128) {
		table_resize(&tab->tab, tab->tab.size / 3);
		printf("SHRINK %li\n", tab->tab.size);
	}
}

int entity_table_init(struct entity_table *tab)
{
	return table_init(&tab->tab, 300000000);
}

static size_t count_links(struct entity_table *tab, size_t bucket)
{
	size_t count = 0;
	struct table_entry *ent = tab->tab.table[bucket];
	while (ent != NULL) {
		ent = ent->next;
		count += 1;
	}
	return count;
}

void tab_stats(struct entity_table *tab)
{
	size_t min, max, i, count;
	double mean, var, diff, inv_size;

	inv_size = 1. / tab->tab.size;
	mean = inv_size * tab->tab.load;
	var = 0;


	min = SIZE_MAX;
	max = 0;
	for (i = 0; i < tab->tab.size; i++) {
		count = count_links(tab, i);
		if (count < min)
			min = count;
		if (count > max)
			max = count;
		diff = count - mean;
		var += inv_size * diff * diff;
	}

	printf("mean %li/%li = %f\n", tab->tab.load, tab->tab.size, mean);
	printf("var %f\n", var);
	printf("min = %li max = %li\n", min, max);
}

int main(void)
{
	struct entity_table tab;
//	struct entity *ent;

	murmur3_seed = 0xdeedbeef;

  	if (entity_table_init(&tab))
		return 1;

#if 0
	ent = entity_new(&tab, -1);
	printf("tab(%p):\n", (void*)&tab);
	hexdump(&tab, sizeof(tab));
	printf("tab.tab.table(%p):\n", (void*)tab.tab.table);
	hexdump(tab.tab.table, tab.tab.size * sizeof(void*));
	printf("ent(%p):\n", (void*)ent);
	hexdump(ent, sizeof(*ent));
#endif

	for (int i = 0; i < 100000000; i++)
		entity_new(&tab, i);
#if 0
	tab_stats(&tab);

	for (int i = 0; i < 100000000; i++)
		entity_free(&tab, i);

	tab_stats(&tab);

	table_destroy(&tab.tab);
#endif
	return 0;
}

