#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include <common/util.h>
#include <common/hash.h>
#include <common/table.h>

typedef struct entity entity_t;
uint32_t g_murmur;

#define DEFAULT_TABLE_SIZE	(16)

struct entity {
	TABLE_ENTRY(entity_t, uint32_t) link;
	uint64_t id;
};

typedef LINKED_TABLE(, entity_t) entity_table_t;

static int entity_cmp(const entity_t *left, const entity_t *right)
{
	return left->id != right->id;
}

static uint32_t entity_hash(const entity_t *ent)
{
	uint32_t hash = g_murmur;
	hash = murmur3_update(hash, ent->id);
	hash = murmur3_update(hash, ent->id >> 32);
	return hash;
}

TABLE_GEN(, et_, entity_table_t, entity_t, link, entity_hash, entity_cmp)

entity_t *entity_get(entity_table_t *tab, uint64_t id)
{
	entity_t key;
	key.id = id;
	return et_first_equal(tab, &key);
}

entity_t *entity_new(entity_table_t *tab, uint64_t id)
{
	entity_t *ent;

	if (entity_get(tab, id) != NULL)
		return NULL;

	ent = malloc(sizeof(*ent));
	if (ent == NULL)
		return NULL;

	ent->id = id;

	if (et_push(tab, ent) < 0) {
		free(ent);
		return NULL;
	}

	return ent;
}

void entity_free(entity_table_t *tab, uint64_t id)
{
	entity_t *ent = entity_get(tab, id);
	et_remove(tab, ent);
	free(ent);
}

void show_entities(entity_table_t *tab)
{
	entity_t *ent;
	for (ent = et_first(tab); ent != NULL; ent = et_next(tab, ent))
		printf("%li\n", ent->id);
}

int main(void)
{
	entity_table_t table = TABLE_INITIALIZER;

	printf("%i\n", !!entity_new(&table, 100));
	printf("%i\n", !!entity_new(&table, 101));
	printf("%i\n", !!entity_new(&table, 102));
	printf("%i\n", !!entity_new(&table, 103));

	printf("\nentities:\n");

	show_entities(&table);

	return 0;
}

