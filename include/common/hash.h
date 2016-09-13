#ifndef COMMON_HASH_H
#define COMMON_HASH_H

#include <stddef.h>
#include <stdint.h>

#define FNV_OFFSET_BASIS	(0xcbf29ce484222325ULL)

uint64_t fnv1a(uint64_t basis, const void *data, size_t count);
uint32_t murmur3(uint32_t basis, const void *data, size_t count);

uint32_t murmur3_update(uint32_t hash, uint32_t data);
uint32_t murmur3_final(uint32_t hash, size_t num_bytes);

uint64_t nth_pow2_prime(int n);
uint64_t pow2_prime(uint64_t value);

#endif
