/* sha1.c
 *
 * Copyright (c) 2016, Eric Chai <electromatter@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>

#include "sha1.h"

static inline uint32_t rol32(uint32_t val, int n)
{
	return (val << n) | (val >> (32 - n));
}

static inline void be64(void *dest, uint64_t val)
{
	*(uint64_t *)dest = __builtin_bswap64(val);
}

static inline void be32(void *dest, uint32_t val)
{
	*(uint32_t *)dest = __builtin_bswap32(val);
}

static inline uint32_t rdbe32(const void *src)
{
	return __builtin_bswap32(*(const uint32_t *)src);
}

static inline uint32_t expand(int i, uint32_t w[16])
{
	uint32_t x;
	x = w[(i + 13) % 16] ^ w[(i + 8) % 16] ^ w[(i + 2) % 16] ^ w[i % 16];
	return rol32(x, 1);
}

static inline void
roundA(int i, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e,
		uint32_t w[16], const uint32_t src[16])
{
	w[i % 16] = rdbe32(&src[i % 16]);
	*e += rol32(*a, 5) + (((*c ^ *d) & *b) ^ *d) + w[i % 16] + 0x5a827999;
	*b = rol32(*b, 30);
}

static inline void
rounda(int i, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e,
		uint32_t w[16])
{
	w[i % 16] = expand(i, w);
	*e += rol32(*a, 5) + (((*c ^ *d) & *b) ^ *d) + w[i % 16] + 0x5a827999;
	*b = rol32(*b, 30);
}

static inline void
roundB(int i, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e,
		uint32_t w[16])
{
	w[i % 16] = expand(i, w);
	*e += rol32(*a, 5) + (*b ^ *c ^ *d) + w[i % 16] + 0x6ed9eba1;
	*b = rol32(*b, 30);
}

static inline void
roundC(int i, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e,
		uint32_t w[16])
{
	w[i % 16] = expand(i, w);
	*e += rol32(*a, 5) + (*b & *c) + (*d & (*b ^ *c));
	*e += w[i % 16] + 0x8f1bbcdc;
	*b = rol32(*b, 30);
}

static inline void
roundD(int i, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d, uint32_t *e,
		uint32_t w[16])
{
	w[i % 16] = expand(i, w);
	*e += rol32(*a, 5) + (*b ^ *c ^ *d) + w[i % 16] + 0xca62c1d6;
	*b = rol32(*b, 30);
}

static void
sha1_block(uint32_t state[5], const unsigned char data[64], uint32_t w[16])
{
	uint32_t a = state[0], b = state[1], c = state[2],
			d = state[3], e = state[4];
	const uint32_t *src = (const uint32_t *)data;

	roundA( 0, &a, &b, &c, &d, &e, w, src);
	roundA( 1, &e, &a, &b, &c, &d, w, src);
	roundA( 2, &d, &e, &a, &b, &c, w, src);
	roundA( 3, &c, &d, &e, &a, &b, w, src);
	roundA( 4, &b, &c, &d, &e, &a, w, src);
	roundA( 5, &a, &b, &c, &d, &e, w, src);
	roundA( 6, &e, &a, &b, &c, &d, w, src);
	roundA( 7, &d, &e, &a, &b, &c, w, src);
	roundA( 8, &c, &d, &e, &a, &b, w, src);
	roundA( 9, &b, &c, &d, &e, &a, w, src);
	roundA(10, &a, &b, &c, &d, &e, w, src);
	roundA(11, &e, &a, &b, &c, &d, w, src);
	roundA(12, &d, &e, &a, &b, &c, w, src);
	roundA(13, &c, &d, &e, &a, &b, w, src);
	roundA(14, &b, &c, &d, &e, &a, w, src);
	roundA(15, &a, &b, &c, &d, &e, w, src);
	rounda(16, &e, &a, &b, &c, &d, w);
	rounda(17, &d, &e, &a, &b, &c, w);
	rounda(18, &c, &d, &e, &a, &b, w);
	rounda(19, &b, &c, &d, &e, &a, w);
	roundB(20, &a, &b, &c, &d, &e, w);
	roundB(21, &e, &a, &b, &c, &d, w);
	roundB(22, &d, &e, &a, &b, &c, w);
	roundB(23, &c, &d, &e, &a, &b, w);
	roundB(24, &b, &c, &d, &e, &a, w);
	roundB(25, &a, &b, &c, &d, &e, w);
	roundB(26, &e, &a, &b, &c, &d, w);
	roundB(27, &d, &e, &a, &b, &c, w);
	roundB(28, &c, &d, &e, &a, &b, w);
	roundB(29, &b, &c, &d, &e, &a, w);
	roundB(30, &a, &b, &c, &d, &e, w);
	roundB(31, &e, &a, &b, &c, &d, w);
	roundB(32, &d, &e, &a, &b, &c, w);
	roundB(33, &c, &d, &e, &a, &b, w);
	roundB(34, &b, &c, &d, &e, &a, w);
	roundB(35, &a, &b, &c, &d, &e, w);
	roundB(36, &e, &a, &b, &c, &d, w);
	roundB(37, &d, &e, &a, &b, &c, w);
	roundB(38, &c, &d, &e, &a, &b, w);
	roundB(39, &b, &c, &d, &e, &a, w);
	roundC(40, &a, &b, &c, &d, &e, w);
	roundC(41, &e, &a, &b, &c, &d, w);
	roundC(42, &d, &e, &a, &b, &c, w);
	roundC(43, &c, &d, &e, &a, &b, w);
	roundC(44, &b, &c, &d, &e, &a, w);
	roundC(45, &a, &b, &c, &d, &e, w);
	roundC(46, &e, &a, &b, &c, &d, w);
	roundC(47, &d, &e, &a, &b, &c, w);
	roundC(48, &c, &d, &e, &a, &b, w);
	roundC(49, &b, &c, &d, &e, &a, w);
	roundC(50, &a, &b, &c, &d, &e, w);
	roundC(51, &e, &a, &b, &c, &d, w);
	roundC(52, &d, &e, &a, &b, &c, w);
	roundC(53, &c, &d, &e, &a, &b, w);
	roundC(54, &b, &c, &d, &e, &a, w);
	roundC(55, &a, &b, &c, &d, &e, w);
	roundC(56, &e, &a, &b, &c, &d, w);
	roundC(57, &d, &e, &a, &b, &c, w);
	roundC(58, &c, &d, &e, &a, &b, w);
	roundC(59, &b, &c, &d, &e, &a, w);
	roundD(60, &a, &b, &c, &d, &e, w);
	roundD(61, &e, &a, &b, &c, &d, w);
	roundD(62, &d, &e, &a, &b, &c, w);
	roundD(63, &c, &d, &e, &a, &b, w);
	roundD(64, &b, &c, &d, &e, &a, w);
	roundD(65, &a, &b, &c, &d, &e, w);
	roundD(66, &e, &a, &b, &c, &d, w);
	roundD(67, &d, &e, &a, &b, &c, w);
	roundD(68, &c, &d, &e, &a, &b, w);
	roundD(69, &b, &c, &d, &e, &a, w);
	roundD(70, &a, &b, &c, &d, &e, w);
	roundD(71, &e, &a, &b, &c, &d, w);
	roundD(72, &d, &e, &a, &b, &c, w);
	roundD(73, &c, &d, &e, &a, &b, w);
	roundD(74, &b, &c, &d, &e, &a, w);
	roundD(75, &a, &b, &c, &d, &e, w);
	roundD(76, &e, &a, &b, &c, &d, w);
	roundD(77, &d, &e, &a, &b, &c, w);
	roundD(78, &c, &d, &e, &a, &b, w);
	roundD(79, &b, &c, &d, &e, &a, w);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
}

void sha1_init(struct sha1_state *ctx)
{
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xc3d2e1f0;
	memset(ctx->block, 0, sizeof(ctx->block));
	ctx->total_size = 0;
}

void sha1_update(struct sha1_state *ctx, const void *buf, size_t size)
{
	const unsigned char *ptr = buf;
	uint32_t scratch[16];
	unsigned int rem, size_rem;

	rem = ctx->total_size % 64;
	size_rem = 64 - rem;
	if (size < size_rem) {
		memcpy(ctx->block + rem, ptr, size);
		ctx->total_size += size;
		return;
	}

	if (rem > 0) {
		memcpy(ctx->block + rem, ptr, size_rem);
		sha1_block(ctx->state, ctx->block, scratch);
		ctx->total_size += size_rem;
		ptr += size_rem;
		size -= size_rem;
	}

	while (size > 64) {
		sha1_block(ctx->state, ptr, scratch);
		ctx->total_size += 64;
		ptr += 64;
		size -= 64;
	}

	memcpy(ctx->block, ptr, size);
	memset(scratch, 0, sizeof(scratch));
	ctx->total_size += size;
}

void sha1_final(unsigned char digest[20], struct sha1_state *ctx)
{
	uint32_t scratch[16];
	int rem = ctx->total_size % 64;
	if (rem <= 55) {
		memset(ctx->block + rem, 0, sizeof(ctx->block) - rem);
		ctx->block[rem] = 0x80;
		be64(ctx->block + 56, ctx->total_size * 8);
		sha1_block(ctx->state, ctx->block, scratch);
	} else {
		memset(ctx->block + rem, 0, sizeof(ctx->block) - rem);
		ctx->block[rem] = 0x80;
		sha1_block(ctx->state, ctx->block, scratch);
		memset(ctx->block, 0, sizeof(ctx->block));
		be64(ctx->block + 56, ctx->total_size * 8);
		sha1_block(ctx->state, ctx->block, scratch);
	}
	be32(digest +  0, ctx->state[0]);
	be32(digest +  4, ctx->state[1]);
	be32(digest +  8, ctx->state[2]);
	be32(digest + 12, ctx->state[3]);
	be32(digest + 16, ctx->state[4]);
	sha1_init(ctx);
}

void sha1(unsigned char digest[20], const void *buf, size_t size)
{
	struct sha1_state ctx;
	sha1_init(&ctx);
	sha1_update(&ctx, buf, size);
	sha1_final(digest, &ctx);
}
