#ifndef COMMON_HASH_H
#define COMMON_HASH_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define FNV_OFFSET_BASIS	(0xcbf29ce484222325ULL)

uint64_t fnv1a(uint64_t basis, const void *data, size_t count);
uint32_t murmur3(uint32_t basis, const void *data, size_t count);

uint32_t murmur3_update(uint32_t hash, uint32_t data);
uint32_t murmur3_final(uint32_t hash, size_t num_bytes);

static inline
uint32_t murmur3_str(uint32_t hash, const char *val)
{
	return murmur3(hash, val, strlen(val));
}

static inline
uint32_t murmur3_u32(uint32_t hash, uint32_t val)
{
	hash = murmur3_update(hash, val);
	return murmur3_final(hash, sizeof(val));
}

static inline
uint32_t murmur3_u64(uint32_t hash, uint64_t val)
{
	hash = murmur3_update(hash, val);
	hash = murmur3_update(hash, val >> 32);
	return murmur3_final(hash, sizeof(val));
}

uint64_t nth_pow2_prime(int n);
uint64_t pow2_prime(uint64_t value);
uint64_t next_pow2_prime(uint64_t value);
uint64_t prev_pow2_prime(uint64_t value);

#endif
