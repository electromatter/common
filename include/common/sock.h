#ifndef COMMON_SOCK_H
#define COMMON_SOCK_H

#include <sys/types.h>

#include <ev.h>

#include <mcp_base/fbuf.h>

enum sock_event {
	SOCK_READ		= 0,
	SOCK_WRITE		= 3,
	SOCK_SHUT_READ		= 1,
	SOCK_SHUT_WRITE		= 2,
};

struct sock;
typedef void (*sock_func)(struct sock *s, enum sock_event event, size_t delta);

struct sock_args {
	int fd;
	sock_func func;
	void *user;

	int has_buffers;
	struct fbuf read_buf, write_buf;
};

/* embed me! */
struct sock {
	/* public (read/write) */
	struct fbuf read_buf, write_buf;
	sock_func func;
	void *user;

	/* public (read only) */
	int fd, can_read, can_write, read_hold;
	ssize_t write_quota;

	/* internal */
	struct ev_io read_watcher, write_watcher;
	int read_dead, write_dead, async;
};

void sock_init(struct sock *s, const struct sock_args *args);
void sock_destroy(struct sock *s);
void sock_args(struct sock_args *args, struct sock *s);

void sock_shutdown(struct sock *s, int how);

void sock_block(struct sock *s, int hold);
void sock_quota(struct sock *s, ssize_t quota);
void sock_commit(struct sock *s);

#endif
