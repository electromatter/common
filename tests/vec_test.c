#include <stdio.h>

#include <common/vec.h>

typedef int item_t;

typedef FLAT_VEC(, item_t) vec_t;

VEC_GEN(, vec_, vec_t, item_t)

void show_vec(vec_t *vec)
{
	size_t i;
	printf("vec size: %li\n", VEC_COUNT(vec));
	for (i = 0; i < VEC_COUNT(vec); i++)
		printf("%i\n", VEC_AT(vec, i));
}

int main(void)
{
	vec_t vec = VEC_INITIALIZER;

	printf("%li\n", VEC_CAPACITY(&vec));

	if (vec_expand(&vec, 100) < 0)
		return -1;

	vec_push(&vec, 0);
	vec_push(&vec, 1);
	vec_push(&vec, 2);
	vec_push(&vec, 3);
	vec_pop(&vec, NULL);
	vec_pop(&vec, NULL);
	vec_pop(&vec, NULL);
	vec_pop(&vec, NULL);

	printf("%li\n", VEC_CAPACITY(&vec));

	show_vec(&vec);

	return 0;
}

