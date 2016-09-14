#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

/* Debug helpers */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

void fhexdump(const void *src, size_t length, FILE *f);
void hexdump(const void *src, size_t length);

/* Pointer arithmetic */
#ifndef containerof
#include <stddef.h>
#include <sys/types.h>

static inline void *psub_or_null(const void *ptr, ssize_t delta)
{
	if (ptr == NULL)
		return NULL;
	return (char *)ptr - delta;
}

#define containerof(ptr, type, member)					\
	((type *)psub_or_null((0 ? &((type *)NULL)->member : (ptr)),	\
			offsetof(type, member)))
#endif

#define relptr(base, type, ptr)					\
	(0 ? (ptr) : ((type *)((char *)0 + ((char *)(ptr) - (char *)(base)))))

#define absptr(base, type, ptr)					\
	(0 ? (ptr) : ((type *)((char *)base + ((char *)(ptr) - (char *)0))))

#include <stdarg.h>
struct fbuf;
struct mcp_parse;

/* returns 0 on success, 1 on error */
int fbuf_vsprintf(struct fbuf *dest, const char *format, va_list args)
#ifdef __GNUC__
	__attribute__ (( __format__ (__printf__, 2, 0)));
#else
;
#endif

/* returns 0 on success, 1 on error */
int fbuf_sprintf(struct fbuf *dest, const char *format, ...)
#ifdef __GNUC__
	__attribute__ (( __format__ (__printf__, 2, 3)));
#else
;
#endif

/* returns 0 on success, 1 on error */
int fbuf_strcat(struct fbuf *dest, const char *src);

void *fbuf_strdup(struct fbuf *src, size_t *size);

void fbuf_hexdump(struct fbuf *src);
void fbuf_fhexdump(struct fbuf *src, FILE *f);

void mcp_start_fbuf(struct mcp_parse *parse, struct fbuf *buf);

typedef const struct tbframe *tempbuf_frame;
tempbuf_frame tempbuf_push(void);
void tempbuf_pop(tempbuf_frame frame);
struct fbuf *tempbuf(void);

int strbufcpy(char *dest, size_t destsize, const char *src);
int bufstrcpy(char *dest, size_t destsize, const char *src, size_t srcsize);
int bufcpy(void *dest, size_t destsize, const void *src, size_t srcsize);
char *bufstrdup(const char *src, size_t srcsize);
void *bufdup(const void *src, size_t srcsize);

struct timespec;
int timespec_cmp(const struct timespec *left, const struct timespec *right);
void timespec_neg(struct timespec *acc);
void timespec_add(struct timespec *acc, const struct timespec *delta);
void timespec_sub(struct timespec *acc, const struct timespec *delta);
void timespec_mul(struct timespec *acc, const struct timespec *delta);
int timespec_to_msec(const struct timespec *x);
double timespec_to_sec(const struct timespec *x);

void java_hexdigits(char *str, const void *digest, size_t size);


#endif
