#ifdef USE_AES_NI

/* TODO test */

#include <stdlib.h>
#include <string.h>
#include <wmmintrin.h>

#include <common/aes.h>

static __m128i AES_128_ASSIST(__m128i temp1, __m128i temp2)
{
	__m128i temp3;
	temp2 = _mm_shuffle_epi32(temp2 ,0xff);
	temp3 = _mm_slli_si128(temp1, 0x4);
	temp1 = _mm_xor_si128(temp1, temp3);
	temp3 = _mm_slli_si128(temp3, 0x4);
	temp1 = _mm_xor_si128(temp1, temp3);
	temp3 = _mm_slli_si128(temp3, 0x4);
	temp1 = _mm_xor_si128(temp1, temp3);
	temp1 = _mm_xor_si128(temp1, temp2);
	return temp1;
}

static void aes_expand128(__m128i *coeff, const void *secret)
{
	__m128i temp1, temp2;

	temp1 = _mm_loadu_si128(secret);
	coeff[0] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1 ,0x1);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[1] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x2);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[2] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x4);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[3] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x8);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[4] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x10);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[5] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x20);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[6] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x40);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[7] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x80);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[8] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x1b);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[9] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1,0x36);
	temp1 = AES_128_ASSIST(temp1, temp2);
	coeff[10] = temp1;
}

static void KEY_192_ASSIST(__m128i* temp1, __m128i * temp2, __m128i * temp3)
{
	__m128i temp4;
	*temp2 = _mm_shuffle_epi32(*temp2, 0x55);
	temp4 = _mm_slli_si128(*temp1, 0x4);
	*temp1 = _mm_xor_si128(*temp1, temp4);
	temp4 = _mm_slli_si128(temp4, 0x4);
	*temp1 = _mm_xor_si128(*temp1, temp4);
	temp4 = _mm_slli_si128(temp4, 0x4);
	*temp1 = _mm_xor_si128(*temp1, temp4);
	*temp1 = _mm_xor_si128(*temp1, *temp2);
	*temp2 = _mm_shuffle_epi32(*temp1, 0xff);
	temp4 = _mm_slli_si128(*temp3, 0x4);
	*temp3 = _mm_xor_si128(*temp3, temp4);
	*temp3 = _mm_xor_si128(*temp3, *temp2);
}

static void aes_expand192(__m128i *coeff, const void *secret)
{
	unsigned char temp[32];
	__m128i temp1, temp2, temp3;

	memcpy(temp, secret, 24);

	temp1 = _mm_loadu_si128((void*)temp);
	temp3 = _mm_loadu_si128((void*)(temp + 16));
	coeff[0] = temp1;
	coeff[1] = temp3;

	temp2=_mm_aeskeygenassist_si128(temp3, 0x1);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[1] = (__m128i)_mm_shuffle_pd((__m128d)coeff[1], (__m128d)temp1, 0);
	coeff[2] = (__m128i)_mm_shuffle_pd((__m128d)temp1,(__m128d)temp3, 1);

	temp2=_mm_aeskeygenassist_si128(temp3, 0x2);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[3] = temp1;
	coeff[4] = temp3;

	temp2=_mm_aeskeygenassist_si128(temp3, 0x4);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[4] = (__m128i)_mm_shuffle_pd((__m128d)coeff[4], (__m128d)temp1, 0);
	coeff[5] = (__m128i)_mm_shuffle_pd((__m128d)temp1, (__m128d)temp3, 1);

	temp2=_mm_aeskeygenassist_si128(temp3, 0x8);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[6] = temp1;
	coeff[7] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x10);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[7] = (__m128i)_mm_shuffle_pd((__m128d)coeff[7], (__m128d)temp1,0);
	coeff[8] = (__m128i)_mm_shuffle_pd((__m128d)temp1, (__m128d)temp3, 1);

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x20);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[9] = temp1;
	coeff[10] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x40);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[10] = (__m128i)_mm_shuffle_pd((__m128d)coeff[10], (__m128d)temp1, 0);
	coeff[11] = (__m128i)_mm_shuffle_pd((__m128d)temp1, (__m128d)temp3, 1);

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x80);
	KEY_192_ASSIST(&temp1, &temp2, &temp3);
	coeff[12] = temp1;
}

static void KEY_256_ASSIST_1(__m128i* temp1, __m128i * temp2)
{
	__m128i temp4;
	*temp2 = _mm_shuffle_epi32(*temp2, 0xff);
	temp4 = _mm_slli_si128(*temp1, 0x4);
	*temp1 = _mm_xor_si128(*temp1, temp4);
	temp4 = _mm_slli_si128(temp4, 0x4);
	*temp1 = _mm_xor_si128(*temp1, temp4);
	temp4 = _mm_slli_si128(temp4, 0x4);
	*temp1 = _mm_xor_si128(*temp1, temp4);
	*temp1 = _mm_xor_si128(*temp1, *temp2);
}

static void KEY_256_ASSIST_2(__m128i* temp1, __m128i * temp3)
{
	__m128i temp2,temp4;
	temp4 = _mm_aeskeygenassist_si128(*temp1, 0x0);
	temp2 = _mm_shuffle_epi32(temp4, 0xaa);
	temp4 = _mm_slli_si128(*temp3, 0x4);
	*temp3 = _mm_xor_si128(*temp3, temp4);
	temp4 = _mm_slli_si128(temp4, 0x4);
	*temp3 = _mm_xor_si128(*temp3, temp4);
	temp4 = _mm_slli_si128(temp4, 0x4);
	*temp3 = _mm_xor_si128(*temp3, temp4);
	*temp3 = _mm_xor_si128(*temp3, temp2);
}

static void aes_expand256(__m128i *coeff, const void *secret)
{
	__m128i temp1, temp2, temp3;

	temp1 = _mm_load_si128(secret);
	temp3 = _mm_load_si128((const void *)((const char *)secret + 16));
	coeff[0] = temp1;
	coeff[1] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x01);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[2] = temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	coeff[3] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x02);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[4] = temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	coeff[5] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x04);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[6] = temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	coeff[7] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x08);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[8] = temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	coeff[9] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x10);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[10] = temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	coeff[11] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x20);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[12] = temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	coeff[13] = temp3;

	temp2 = _mm_aeskeygenassist_si128(temp3, 0x40);
	KEY_256_ASSIST_1(&temp1, &temp2);
	coeff[14] = temp1;
}

int aes_prepare_encrypt(struct aes_key *key, const void *secret, int bits)
{
	__m128i coeff[15], *dest = (void *)key->coeff;
	int i;

	if (key == NULL)
		return -1;

	if (secret == NULL)
		return -2;

	switch (bits) {
	case 128:
		key->rounds = 10;
		aes_expand128(coeff, secret);
		memcpy(key->secret, secret, 16);
		break;
	case 192:
		key->rounds = 12;
		aes_expand192(coeff, secret);
		memcpy(key->secret, secret, 24);
		break;
	case 256:
		key->rounds = 14;
		aes_expand256(coeff, secret);
		memcpy(key->secret, secret, 32);
		break;
	default:
		return -3;
	}

	for (i = 0; i <= key->rounds; i++)
		_mm_storeu_si128(dest + i, coeff[i]);

	return 0;
}

int aes_prepare_decrypt(struct aes_key *key, const void *secret, int bits)
{
	__m128i a, b, *coeff = (void *)key->coeff;
	int i, j, err;
	err = aes_prepare_encrypt(key, secret, bits);
	if (err)
		return err;

	i = 0, j = key->rounds;
	a = _mm_loadu_si128(coeff + i);
	b = _mm_loadu_si128(coeff + j);
	_mm_storeu_si128(coeff + i, b);
	_mm_storeu_si128(coeff + j, a);
	while (++i <= --j) {
		a = _mm_loadu_si128(coeff + i);
		b = _mm_loadu_si128(coeff + j);
		a = _mm_aesimc_si128(a);
		b = _mm_aesimc_si128(b);
		_mm_storeu_si128(coeff + i, b);
		_mm_storeu_si128(coeff + j, a);
	}
	return 0;
}

static void load_coeff(__m128i *coeff, const struct aes_key *key)
{
	__m128i *src = (void *)key->coeff;
	int i;
	for (i = 0; i <= key->rounds; i++)
		coeff[i] = _mm_loadu_si128(src + i);
}

static __m128i do_rounds_enc(__m128i block, const __m128i *coeff, int rounds)
{
	int i;
	block = _mm_xor_si128(block, coeff[0]);
	for (i = 1; i < rounds; i++)
		block = _mm_aesenc_si128(block, coeff[i]);
	block = _mm_aesenclast_si128(block, coeff[rounds]);
	return block;
}

static __m128i do_rounds_dec(__m128i block, const __m128i *coeff, int rounds)
{
	int i;
	block = _mm_xor_si128(block, coeff[0]);
	for (i = 1; i < rounds; i++)
		block = _mm_aesdec_si128(block, coeff[i]);
	block = _mm_aesdeclast_si128(block, coeff[rounds]);
	return block;
}

void aes_encrypt_block(void *ciphertext, const void *cleartext,
		const struct aes_key *key)
{
	__m128i block, coeff[15];
	load_coeff(coeff, key);
	block = _mm_loadu_si128(cleartext);
	switch (key->rounds) {
	case 10:
		block = do_rounds_enc(block, coeff, 10);
		break;
	case 12:
		block = do_rounds_enc(block, coeff, 12);
		break;
	case 14:
		block = do_rounds_enc(block, coeff, 14);
		break;
	default:
		abort();
	}
	_mm_storeu_si128(ciphertext, block);
}

void aes_decrypt_block(void *cleartext, const void *ciphertext,
		const struct aes_key *key)
{
	__m128i block, coeff[15];
	load_coeff(coeff, key);
	block = _mm_loadu_si128(ciphertext);
	switch (key->rounds) {
	case 10:
		block = do_rounds_dec(block, coeff, 10);
		break;
	case 12:
		block = do_rounds_dec(block, coeff, 12);
		break;
	case 14:
		block = do_rounds_dec(block, coeff, 14);
		break;
	default:
		abort();
	}
	_mm_storeu_si128(cleartext, block);
}

void aes_ctr_block(void *ciphertext, size_t ctr, const struct aes_key *key)
{
	__m128i block, coeff[15];
	load_coeff(coeff, key);
	block = _mm_set_epi64x(0, __builtin_bswap64(ctr));
	switch (key->rounds) {
	case 10:
		block = do_rounds_enc(block, coeff, 10);
		break;
	case 12:
		block = do_rounds_enc(block, coeff, 12);
		break;
	case 14:
		block = do_rounds_enc(block, coeff, 14);
		break;
	default:
		abort();
	}
	_mm_storeu_si128(ciphertext, block);
}

int aes_prepare_stream(struct aes_stream_key *key,
		const void *secret, int bits,
		const void *feedback_iv)
{
	int err;
	err = aes_prepare_encrypt(&key->key, secret, bits);
	if (err)
		return err;
	memcpy(key->feedback, feedback_iv, 16);
	key->total_bytes = 0;
	return 0;
}

void aes_encrypt_cfb8(void *ciphertext, const void *cleartext, size_t nbytes,
		struct aes_stream_key *key)
{
	__m128i feedback, temp, coeff[15];
	unsigned char *dest = ciphertext;
	const unsigned char *src = cleartext;
	int byte;

	load_coeff(coeff, &key->key);
	feedback = _mm_loadu_si128((void*)key->feedback);
	key->total_bytes += nbytes;

	switch (key->key.rounds) {
	case 10:
		while (nbytes --> 0) {
			temp = do_rounds_enc(feedback, coeff, 10);
			byte = (temp[0] ^ *src++) & 0xff;
			*dest++ = byte;
			feedback = _mm_srli_si128(feedback, 1);
			feedback ^= _mm_set_epi8(byte, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		break;
	case 12:
		while (nbytes --> 0) {
			temp = do_rounds_enc(feedback, coeff, 12);
			byte = (temp[0] ^ *src++) & 0xff;
			*dest++ = byte;
			feedback = _mm_srli_si128(feedback, 1);
			feedback ^= _mm_set_epi8(byte, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		break;
	case 14:
		while (nbytes --> 0) {
			temp = do_rounds_enc(feedback, coeff, 14);
			byte = (temp[0] ^ *src++) & 0xff;
			*dest++ = byte;
			feedback = _mm_srli_si128(feedback, 1);
			feedback ^= _mm_set_epi8(byte, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		break;
	default:
		abort();
	}
	_mm_storeu_si128((void*)key->feedback, feedback);
}

void aes_decrypt_cfb8(void *cleartext, const void *ciphertext, size_t nbytes,
		struct aes_stream_key *key)
{
	__m128i feedback, temp, coeff[15];
	unsigned char *dest = cleartext;
	const unsigned char *src = ciphertext;
	int byte;

	load_coeff(coeff, &key->key);
	feedback = _mm_loadu_si128((void*)key->feedback);
	key->total_bytes += nbytes;

	switch (key->key.rounds) {
	case 10:
		while (nbytes --> 0) {
			temp = do_rounds_enc(feedback, coeff, 10);
			byte = *src++;
			*dest++ = (temp[0] ^ byte) & 0xff;
			feedback = _mm_srli_si128(feedback, 1);
			feedback = _mm_xor_si128(feedback, _mm_set_epi8(byte, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
		}
		break;
	case 12:
		while (nbytes --> 0) {
			temp = do_rounds_enc(feedback, coeff, 12);
			byte = *src++;
			*dest++ = (temp[0] ^ byte) & 0xff;
			feedback = _mm_srli_si128(feedback, 1);
			feedback = _mm_xor_si128(feedback, _mm_set_epi8(byte, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
		}
		break;
	case 14:
		while (nbytes --> 0) {
			temp = do_rounds_enc(feedback, coeff, 14);
			byte = *src++;
			*dest++ = (temp[0] ^ byte) & 0xff;
			feedback = _mm_srli_si128(feedback, 1);
			feedback = _mm_xor_si128(feedback, _mm_set_epi8(byte, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
		}
		break;
	default:
		abort();
	}
	_mm_storeu_si128((void*)key->feedback, feedback);
}

#else

#include <string.h>
#include <assert.h>

#include <openssl/aes.h>

#include <common/aes.h>

int aes_prepare_encrypt(struct aes_key *key, const void *secret, int bits)
{
	AES_KEY *ctx = (void*)key->coeff;

	assert(sizeof(key->coeff) >= sizeof(*ctx));

	switch (bits) {
	case 128:
		key->rounds = 10;
		memcpy(key->secret, secret, 16);
		break;
	case 192:
		key->rounds = 12;
		memcpy(key->secret, secret, 24);
		break;
	case 256:
		key->rounds = 14;
		memcpy(key->secret, secret, 32);
		break;
	default:
		return -1;
	}

	if (AES_set_encrypt_key(secret, bits, ctx))
		return -2;

	return 0;
}

int aes_prepare_decrypt(struct aes_key *key, const void *secret, int bits)
{
	AES_KEY *ctx = (void*)key->coeff;

	assert(sizeof(key->coeff) >= sizeof(*ctx));

	switch (bits) {
	case 128:
		key->rounds = 10;
		memcpy(key->secret, secret, 16);
		break;
	case 192:
		key->rounds = 12;
		memcpy(key->secret, secret, 24);
		break;
	case 256:
		key->rounds = 14;
		memcpy(key->secret, secret, 32);
		break;
	default:
		return -1;
	}

	if (AES_set_decrypt_key(secret, bits, ctx))
		return -2;

	return 0;
}

int aes_prepare_stream(struct aes_stream_key *key,
		const void *secret, int bits,
		const void *feedback_iv)
{
	int err;
	err = aes_prepare_encrypt(&key->key, secret, bits);
	if (err)
		return err;
	memcpy(key->feedback, feedback_iv, 16);
	key->total_bytes = 0;
	return 0;
}

void aes_encrypt_block(void *ciphertext, const void *cleartext,
		const struct aes_key *key)
{
	AES_KEY *ctx = (void*)key->coeff;
	AES_encrypt(cleartext, ciphertext, ctx);
}

void aes_decrypt_block(void *cleartext, const void *ciphertext,
		const struct aes_key *key)
{
	AES_KEY *ctx = (void*)key->coeff;
	AES_decrypt(ciphertext, cleartext, ctx);
}

void aes_ctr_block(void *ciphertext, size_t ctr, const struct aes_key *key)
{
	char block[AES_BLOCK_SIZE];
	ctr = __builtin_bswap64(ctr);
	memcpy(block - sizeof(ctr), &ctr, sizeof(ctr));
	aes_encrypt_block(ciphertext, block, key);
}

void aes_encrypt_cfb8(void *ciphertext, const void *cleartext, size_t nbytes,
		struct aes_stream_key *key)
{
	int num = 0;
	AES_KEY *ctx = (void*)key->key.coeff;
	AES_cfb8_encrypt(cleartext, ciphertext, nbytes, ctx, (void*)key->feedback, &num, 1);
	key->total_bytes += nbytes;
}

void aes_decrypt_cfb8(void *cleartext, const void *ciphertext, size_t nbytes,
		struct aes_stream_key *key)
{
	int num = 0;
	AES_KEY *ctx = (void*)key->key.coeff;
	AES_cfb8_encrypt(ciphertext, cleartext, nbytes, ctx, (void*)key->feedback, &num, 0);
	key->total_bytes += nbytes;
}

#endif
