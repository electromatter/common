#include <common/util.h>

void java_hexdigits(char *str, const void *digest, size_t size)
{
	char *base = str;
	const unsigned char *d = digest;
	const char hex[16] = "0123456789abcdef";
	size_t i, j;
	int carry;

	if (size == 0) {
		*str++ = '0';
		*str++ = 0;
		return;
	}

	if (!(d[0] & 0x80)) {
		/* skip leading zeros */
		for (i = 0; i < size - 1 && d[i] == 0; i++);
		if ((d[i] & 0xf0) == 0) {
			*str++ = hex[d[i]];
			i++;
		}

		/* format the rest of the bytes */
		for (; i < size; i++) {
			*str++ = hex[d[i] >> 4];
			*str++ = hex[d[i] & 0xf];
		}

		*str++ = 0;
		return;
	}

	/* here, we find the two's complement of digest in reverse. */
	*str++ = 0;

	/* find the first non zero leading byte */
	for (j = 0; j < size - 1 && d[j] == 0xff; j++);

	/* preform the two's complement */
	carry = 1;
	for (i = size; i --> j;) {
		carry += (~d[i]) & 0xff;
		*str++ = hex[carry & 0xf];
		*str++ = hex[(carry >> 4) & 0xf];
		carry >>= 8;
	}

	/* that last digit could have been a leading zero, trim it. */
	if (str[-1] == '0')
		str--;

	*str++ = '-';

	/* reverse the string */
	while (base < str) {
		carry = *--str;
		*str = *base;
		*base++ = carry;
	}
}
