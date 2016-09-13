#include <stdlib.h>
#include <stdint.h>

#include <common/hash.h>

uint64_t fnv1a(uint64_t hash, const void *data, size_t count)
{
	const unsigned char *ptr = data, *end = ptr + count;
	while (ptr < end)
		hash = (hash ^ *ptr++) * 0x100000001b3ULL;
	return hash;
}

static inline
uint32_t rol32(uint32_t x, unsigned int y)
{
	return (x << y) | (x >> (8 * sizeof(x) - y));
}

uint32_t murmur3_update(uint32_t hash, uint32_t data)
{
	hash ^= rol32(data * 0xcc9e2d51, 15) * 0x1b873593;
	return rol32(hash, 13) * 3 + 0xe6546b64;
}

uint32_t murmur3_final(uint32_t hash, size_t num_bytes)
{
	hash ^= num_bytes;
	hash = (hash ^ (hash >> 16)) * 0x85ebca6b;
	hash = (hash ^ (hash >> 13)) * 0xc2b2ae35;
	return hash ^ (hash >> 16);
}

uint32_t murmur3(uint32_t hash, const void *data, size_t count)
{
	uint32_t rem;
	const uint32_t *ptr = data, *end = ptr + count / 4;
	const unsigned char *tail = (void *)end;

	while (ptr < end)
		hash = murmur3_update(hash, *ptr++);

	rem = 0;
	switch (count % 4) {
	case 3:
		rem ^= tail[2] << 16;
	case 2:
		rem ^= tail[1] << 8;
	case 1:
		rem ^= tail[0];
		hash ^= rol32(rem * 0xcc9e2d51, 15) * 0x1b873593;
	}

	return murmur3_final(hash, count);
}

static const unsigned char residuals[] = {
	0x01, 0x01, 0x01, 0x03, 0x01, 0x05, 0x03, 0x03,
	0x01, 0x09, 0x07, 0x05, 0x03, 0x11, 0x1b, 0x03,
	0x01, 0x1d, 0x03, 0x15, 0x07, 0x11, 0x0f, 0x09,
	0x2b, 0x23, 0x0f, 0x1d, 0x03, 0x0b, 0x03, 0x0b,
	0x0f, 0x11, 0x19, 0x35, 0x1f, 0x09, 0x07, 0x17,
	0x0f, 0x1b, 0x0f, 0x1d, 0x07, 0x3b, 0x0f, 0x05,
	0x15, 0x45, 0x37, 0x15, 0x15, 0x05, 0x9f, 0x03,
	0x51, 0x09, 0x45, 0x83, 0x21, 0x0f, 0x87, 0x1d,
};

uint64_t nth_pow2_prime(int n)
{
	if (n <= 0)
		return 2;
	if (n >= 64)
		return 0xffffffffffffffc5ULL;
	return (1ULL << n) + residuals[n];
}

uint64_t pow2_prime(uint64_t value)
{
	int left = 0, mid, right = 65;
	while (left <= right) {
		mid = (left + right) / 2;
		if (nth_pow2_prime(mid) >= value)
			right = mid - 1;
		else
			left = mid + 1;
	}
	return nth_pow2_prime(left);
}
