#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/sha1.h>
#include <common/util.h>
#include <common/uuid.h>

static int hex_dec(int x)
{
	if (x >= '0' && x <= '9')
		return x - '0';
	if (x >= 'a' && x <= 'f')
		return x - 'a' + 10;
	if (x >= 'A' && x <= 'F')
		return x - 'A' + 10;
	return -1;
}

int parse_uuid(unsigned char uuid[16], const char *text)
{
	uint64_t a, b, c, d, e;
	int i, j;

	if (sscanf(text, "%lx-%lx-%lx-%lx-%lx", &a, &b, &c, &d, &e) !=  5)
		return -1;

	if (a > 0xFFFFFFFFUL || b > 0xFFFF || c > 0xFFFF || d > 0xFFFF)
		return -1;

	if (e > 0xFFFFFFFFFFFFUL)
		return -1;

	for (j = 24, i = 0; i < 4; i++, j -= 8)
		uuid[i] = a >> j;
	for (j = 8; i < 6; i++, j -= 8)
		uuid[i] = b >> j;
	for (j = 8; i < 8; i++, j -= 8)
		uuid[i] = c >> j;
	for (j = 8; i < 10; i++, j -= 8)
		uuid[i] = d >> j;
	for (j = 40; i < 16; i++, j -= 8)
		uuid[i] = e >> j;

	return 0;
}

int parse_hex_uuid(unsigned char uuid[16], const char *text)
{
	int i = strlen(text), j = 16, ret;
	if (i < 1 || i > 32)
		return -1;
	while (i --> 0) {
		ret = hex_dec(text[i]);
		if (ret < 0)
			return -1;
		if (i & 1) {
			uuid[--j] = ret;
		} else {
			uuid[j] |= (ret << 4);
		}
	}
	while (j--) {
		uuid[j] = 0;
	}
	return 0;
}

void format_uuid(char text[37], const unsigned char uuid[16])
{
	const char hex[] = "0123456789abcdef";
	int i = 0, j = 0;
	while (j < 4) {
		text[i++] = hex[(uuid[j] >> 4) & 0xf];
		text[i++] = hex[uuid[j++] & 0xf];
	}
	text[i++] = '-';
	while (j < 6) {
		text[i++] = hex[(uuid[j] >> 4) & 0xf];
		text[i++] = hex[uuid[j++] & 0xf];
	}
	text[i++] = '-';
	while (j < 8) {
		text[i++] = hex[(uuid[j] >> 4) & 0xf];
		text[i++] = hex[uuid[j++] & 0xf];
	}
	text[i++] = '-';
	while (j < 10) {
		text[i++] = hex[(uuid[j] >> 4) & 0xf];
		text[i++] = hex[uuid[j++] & 0xf];
	}
	text[i++] = '-';
	while (j < 16) {
		text[i++] = hex[(uuid[j] >> 4) & 0xf];
		text[i++] = hex[uuid[j++] & 0xf];
	}
	text[i++] = 0;
}

int canonicalize_uuid(char uuid_text[37])
{
	unsigned char tmp[16] = {0};
	if (parse_uuid(tmp, uuid_text) < 0 && parse_hex_uuid(tmp, uuid_text) < 0)
		return -1;
	format_uuid(uuid_text, tmp);
	return 0;
}

int uuid5(char uuid[37], const char nsuuid[37], const void *name, size_t size)
{
	struct sha1_state hash;
	unsigned char ns[16], digest[20];

	if (parse_uuid(ns, nsuuid) < 0)
		return -1;

	sha1_init(&hash);
	sha1_update(&hash, ns, sizeof(ns));
	sha1_update(&hash, name, size);
	sha1_final(digest, &hash);

	digest[6] &= 0xf0;
	digest[6] |= 0x50;
	digest[8] &= 0x30;
	digest[8] |= 0x80;

	format_uuid(uuid, digest);
	return 0;
}

