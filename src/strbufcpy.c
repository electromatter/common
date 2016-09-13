#include <string.h>

#include <common/util.h>

int strbufcpy(char *dest, size_t destsize, const char *src)
{
	dest[destsize - 1] = 0;
	strncpy(dest, src, destsize);
	if (dest[destsize - 1] != 0) {
		dest[destsize - 1] = 0;
		return -1;
	}
	return 0;
}

int bufstrcpy(char *dest, size_t destsize, const char *src, size_t srcsize)
{
	size_t size = destsize - 1;
	if (srcsize < size)
		size = srcsize;
	dest[destsize - 1] = 0;
	strncpy(dest, src, size);
	return size == srcsize ? 0 : -1;
}

char *bufstrdup(const char *src, size_t srcsize)
{
	char *buf;
	const char *end = memchr(src, 0, srcsize);
	if (end != NULL)
		srcsize = end - src;
	buf = malloc(srcsize + 1);
	if (buf == NULL)
		return NULL;
	memcpy(buf, src, srcsize);
	buf[srcsize] = 0;
	return buf;
}

void *bufdup(const void *src, size_t srcsize)
{
	void *buf = malloc(srcsize + 1);
	if (buf == NULL)
		return NULL;
	memcpy(buf, src, srcsize);
	*((char*)buf + srcsize) = 0;
	return buf;
}

