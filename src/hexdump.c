#include <stdio.h>

#include <common/util.h>

static const char hex[] = "0123456789abcdef";

static char *bhex(char *base, int value)
{
	*base++ = hex[(value >> 4) & 0x0f];
	*base++ = hex[ value       & 0x0f];
	return base;
}

static char *boffset(char *base, int size, size_t value)
{
	switch (size) {
	case 4:
		*base++ = hex[(value >> 60) & 0x0f];
		*base++ = hex[(value >> 56) & 0x0f];
	case 3:
		*base++ = hex[(value >> 52) & 0x0f];
		*base++ = hex[(value >> 48) & 0x0f];
	case 2:
		*base++ = hex[(value >> 44) & 0x0f];
		*base++ = hex[(value >> 40) & 0x0f];
	case 1:
		*base++ = hex[(value >> 36) & 0x0f];
		*base++ = hex[(value >> 32) & 0x0f];
	case 0:
	default:
		*base++ = hex[(value >> 28) & 0x0f];
		*base++ = hex[(value >> 24) & 0x0f];
		*base++ = hex[(value >> 20) & 0x0f];
		*base++ = hex[(value >> 16) & 0x0f];
		*base++ = hex[(value >> 12) & 0x0f];
		*base++ = hex[(value >>  8) & 0x0f];
		*base++ = hex[(value >>  4) & 0x0f];
		*base++ = hex[ value        & 0x0f];
	}
	return base;
}

void fhexdump(const void *src, size_t length, FILE *f)
{
	char line[88], *dest;
	const char *ptr = src, *end = ptr + length;
	int size = 0, col;

	while ((length >> (32 + 8 * size)) > 0)
		size++;

	if (f == NULL)
		f = stdout;

	while (ptr < end) {
		dest = boffset(line, size, ptr - (const char *)src);
		*dest++ = ' '; *dest++ = ' ';

		for (col = 0; col < 8 && ptr + col < end; col++) {
			dest = bhex(dest, ptr[col]);
			*dest++ = ' ';
		}

		*dest++ = ' ';

		for (; col < 16 && ptr + col < end; col++) {
			dest = bhex(dest, ptr[col]);
			*dest++ = ' ';
		}

		for (; col < 16; col++) {
			*dest++ = ' '; *dest++ = ' '; *dest++ = ' ';
		}

		*dest++ = ' ';
		*dest++ = '|';

		for (col = 0; col < 16 && ptr + col < end; col++)
			if (ptr[col] >= 32 && ptr[col] <= 126)
				*dest++ = ptr[col];
			else
				*dest++ = '.';

		*dest++ = '|';
		*dest++ = '\n';
		*dest++ = '\0';

		ptr += 16;
		fputs(line, f);
	}

	dest = boffset(line, size, length);
	*dest++ = '\n';
	*dest++ = '\0';

	fputs(line, f);
}

void hexdump(const void *src, size_t length)
{
	fhexdump(src, length, NULL);
}
