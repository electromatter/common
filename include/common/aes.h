#ifndef COMMON_AES_H
#define COMMON_AES_H

#include <stdlib.h>

#ifndef AES_BLOCK_SIZE
# define AES_BLOCK_SIZE		(16)
#endif

struct aes_key {
	unsigned char coeff[256], secret[32];
	int rounds;
};

struct aes_stream_key {
	struct aes_key key;
	unsigned char feedback[16];
	size_t total_bytes;
};

int aes_prepare_encrypt(struct aes_key *key, const void *secret, int bits);
int aes_prepare_decrypt(struct aes_key *key, const void *secret, int bits);
int aes_prepare_stream(struct aes_stream_key *key,
		const void *secret, int bits,
		const void *feedback_iv);

void aes_encrypt_block(void *ciphertext, const void *cleartext,
		const struct aes_key *key);
void aes_decrypt_block(void *cleartext, const void *ciphertext,
		const struct aes_key *key);

void aes_ctr_block(void *ciphertext, size_t ctr, const struct aes_key *key);

void aes_encrypt_cfb8(void *ciphertext, const void *cleartext, size_t nbytes,
		struct aes_stream_key *key);
void aes_decrypt_cfb8(void *cleartext, const void *ciphertext, size_t nbytes,
		struct aes_stream_key *key);

#endif
