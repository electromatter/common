#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "util.h"

#if 0
void *worker(void *arg)
{
	return arg;
}

int main(int argc, char **argv)
{
	pthread_t tid;
	int i;
	(void)argc; (void)argv;

	for (i = 0; i < 4; i++)
		assert(pthread_create(&tid, NULL, worker, NULL) == 0);

	while (1)
		pause();
}
#endif

#include "aes.h"

#if 0
int main(int argc, char **argv)
{
	unsigned char key[] = {
#if 1
0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
#elif 0
0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b,
#elif 0
0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
#endif
	};
	unsigned char test[] = {
0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10,
	};
	char out[16], out2[16];
	unsigned int i, ret;
	struct aes_key en, de;

	(void)argc; (void)argv;

	ret = 0;
	ret |= aes_prepare_encrypt(&en, key, sizeof(key) * 8);
	ret |= aes_prepare_decrypt(&de, key, sizeof(key) * 8);

	if (ret)
		return 1;

	for (i = 0; i < sizeof(test); i += 16) {
		aes_encrypt_block(out, test + i, &en);
		aes_decrypt_block(out2, out, &de);
		printf("clear\n");
		hexdump(test + i, 16, NULL);
		printf("decrypted\n");
		hexdump(out2, 16, NULL);
		printf("encrypted\n");
		hexdump(out, 16, NULL);
	}

}
#elif 0

#include "string.h"

int main(int argc, char **argv)
{
	unsigned char key[] = {
0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
	};
	//unsigned char test[16] = "test";
	unsigned char iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,};
	//char out[16];//, out2[16];
	//size_t i, ret;
	size_t ret;
	struct aes_stream_key en, de;

	(void)argc; (void)argv;

	ret = 0;
	ret |= aes_prepare_stream(&en, key, sizeof(key) * 8, iv);
	ret |= aes_prepare_stream(&de, key, sizeof(key) * 8, iv);

	if (ret)
		return 1;

	char *a = malloc(16*1024), *b = malloc(16*1024);

	memset(a, 0xff, 16*1024);
	printf("done\n");

	size_t i;
	for (i = 0; i < 1024*1024/16; i++)
		aes_encrypt_cfb8(b, a, 16*1024, &en);

	return 0;
}
#elif 0
#include "openssl/evp.h"
#include <openssl/crypto.h>
#include <openssl/modes.h>
#include <openssl/aes.h>
#include <stdlib.h>
#include <string.h>

static void cfbr_encrypt_block(const unsigned char *in, unsigned char *out,
                               const void *key,
                               unsigned char ivec[16])
{
	unsigned char ovec[16 * 2 + 1];
	memcpy(ovec, ivec, 16);
	AES_encrypt(ivec, ivec, key);
	out[0] = (ovec[16] = in[0] ^ ivec[0]);
	memcpy(ivec, ovec + 1, 16);
 }

void cfb8(const unsigned char *in, unsigned char *out,
                             size_t length, const void *key,
                             unsigned char ivec[16])
{
    size_t n;
    for (n = 0; n < length; ++n)
        cfbr_encrypt_block(&in[n], &out[n], key, ivec);
}

int main(int argc, char **argv)
{
	unsigned char ukey[] = {
0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
	};
	//unsigned char test[16] = "test";
	unsigned char iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,};
	//char out[16];//, out2[16];
	AES_KEY key;
	(void)argc; (void)argv;

	AES_set_encrypt_key(ukey, 128, &key);

	char *a = malloc(16*1024), *b = malloc(16*1024);

	memset(a, 0xff, 16*1024);
	printf("done\n");

	size_t i;
	for (i = 0; i < 1024*1024/16; i++)
		cfb8((void*)a, (void*)b, 16*1024, &key, iv);

	return 0;
}

#elif 1

#include <unistd.h>
#include <fcntl.h>

#include "aes.h"

int main(int argc, char **argv)
{
	unsigned char ukey[16], block[16];
	size_t ctr;
	struct aes_key key;
	int fd = open("/dev/random", O_RDONLY), off, ret;
	(void)argc; (void)argv;
	if (fd < 0)
		return 1;

	while (1) {
		off = 0;
		while (1) {
			ret = read(fd, ukey + off, sizeof(ukey) - off);
			if (ret <= 0)
				return 2;
			off += ret;
			if (off >= (int)sizeof(ukey))
				break;
		}
		if (aes_prepare_encrypt(&key, ukey, 128))
			return 3;
		for (ctr = 0; ctr < 1024 * 1024 / sizeof(block); ctr++) {
			aes_ctr_block(block, ctr, &key);
			if (write(STDOUT_FILENO, block, sizeof(block)) != sizeof(block))
				return 4;
		}
	}

}

#else

#include "sha1.h"

int main(int argc, char **argv)
{
	char digest[42];
	(void)argc; (void)argv;
/*
sha1(Notch) :  4ed1f46bbe04bc756bcb17c0c7ce3e4632f06a48
sha1(jeb_)  : -7c9d5b0044c130109a5d7b5fb5c317c02b4e28c1
sha1(simon) :  88e16a1019277b15d58faf0541e11910eb756f6
 */
	sha1((void*)digest, "abc", 3);
	hexdump(digest, 20, NULL);
	java_sha1(digest, "Notch", 5);
	printf("Notch: %s\n", digest);
	java_sha1(digest, "jeb_", 4);
	printf("jeb_: %s\n", digest);
	java_sha1(digest, "simon", 5);
	printf("simon: %s\n", digest);
	return 0;
}

#endif
