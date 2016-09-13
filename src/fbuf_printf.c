#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <mcp_base/fbuf.h>
#include <mcp_base/mcp.h>

#include "util.h"

int fbuf_vsprintf(struct fbuf *dest, const char *format, va_list args)
{
	va_list temp;
	char *ptr;
	int ret;

	va_copy(temp, args);
	ptr = (char*)fbuf_wptr(dest, 0);
	ret = vsnprintf(ptr, fbuf_wavail(dest), format, temp);
	va_end(temp);

	if (ret < 0) {
		va_end(args);
		return 1;
	}

	if ((size_t)ret <= fbuf_wavail(dest)) {
		fbuf_produce(dest, ret);
		va_end(args);
		return 0;
	}

	ptr = (char*)fbuf_wptr(dest, ret);
	if (ptr == NULL) {
		va_end(args);
		return 1;
	}

	ret = vsnprintf(ptr, fbuf_wavail(dest), format, args);
	va_end(args);

	if (ret < 0)
		return 1;

	fbuf_produce(dest, ret);
	return 0;
}

int fbuf_sprintf(struct fbuf *dest, const char *format, ...)
{
	int ret;
	va_list args;

	va_start(args, format);
	ret = fbuf_vsprintf(dest, format, args);
	va_end(args);

	return ret;
}

int fbuf_strcat(struct fbuf *dest, const char *src)
{
	size_t len = strlen(src);
	void *ptr = fbuf_wptr(dest, len);
	if (ptr == NULL)
		return 1;
	memcpy(ptr, src, len);
	fbuf_produce(dest, len);
	return 0;
}

void *fbuf_strdup(struct fbuf *src, size_t *size)
{
	void *dest = malloc(fbuf_avail(src) + 1);

	if (dest == NULL)
		return NULL;

	if (size == NULL)
		*size = fbuf_avail(src);

	memcpy(dest, fbuf_ptr(src), fbuf_avail(src));
	*((char*)dest + fbuf_avail(src)) = 0;

	return dest;
}

void fbuf_hexdump(struct fbuf *src)
{
	hexdump(fbuf_ptr(src), fbuf_avail(src));
}

void fbuf_fhexdump(struct fbuf *src, FILE *f)
{
	fhexdump(fbuf_ptr(src), fbuf_avail(src), f);
}

void mcp_start_fbuf(struct mcp_parse *parse, struct fbuf *buf)
{
	mcp_start(parse, fbuf_ptr(buf), fbuf_avail(buf));
}
