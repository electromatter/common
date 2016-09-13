#include <stdlib.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <unistd.h>

#include "net.h"
#include "sock.h"

#define IO_SIZE		(64*1024)

extern struct ev_loop *g_loop;

static void sock_do_read(struct ev_loop *loop, struct ev_io *w, int revents);
static void sock_do_write(struct ev_loop *loop, struct ev_io *w, int revents);

void sock_init(struct sock *s, const struct sock_args *args)
{
	fbuf_init(&s->read_buf, FBUF_MAX);
	fbuf_init(&s->write_buf, FBUF_MAX);
	s->func = args->func;
	s->user = args->user;

	s->fd = args->fd;
	s->can_read = 1;
	s->can_write = 1;
	s->read_hold = 1;
	s->write_quota = -1;

	ev_io_init(&s->read_watcher, sock_do_read, args->fd, EV_READ);
	ev_io_init(&s->write_watcher, sock_do_write, args->fd, EV_WRITE);
	s->read_watcher.data = s;
	s->write_watcher.data = s;

	s->read_dead = 0;
	s->write_dead = 0;
	s->async = 0;

	if (args->has_buffers) {
		s->read_buf = args->read_buf;
		s->write_buf = args->write_buf;
	}

	sock_block(s, 0);
}

void sock_destroy(struct sock *s)
{
	if (s == NULL)
		return;

	assert(s->read_watcher.fd == s->fd);
	assert(s->write_watcher.fd == s->fd);
	assert(s->fd >= 0);

	ev_io_stop(g_loop, &s->read_watcher);
	ev_io_stop(g_loop, &s->write_watcher);

	close(s->fd);

	fbuf_free(&s->read_buf);
	fbuf_free(&s->write_buf);
	s->fd = -1;
	s->can_read = 0;
	s->can_write = 0;
	s->read_hold = 0;
	s->write_quota = -1;
	s->read_watcher.fd = -1;
	s->write_watcher.fd = -1;
	s->read_dead = 1;
	s->write_dead = 1;
}

void sock_args(struct sock_args *args, struct sock *s)
{
	assert(s->read_watcher.fd == s->fd);
	assert(s->write_watcher.fd == s->fd);
	assert(s->fd >= 0);

	ev_io_stop(g_loop, &s->read_watcher);
	ev_io_stop(g_loop, &s->write_watcher);

	args->fd = s->fd;
	args->func = s->func;
	args->user = s->user;
	args->has_buffers = 1;
	args->read_buf = s->read_buf;
	args->write_buf = s->write_buf;

	/* clear out s so we can detect use after free (sock_args) */
	fbuf_init(&s->read_buf, FBUF_MAX);
	fbuf_init(&s->write_buf, FBUF_MAX);
	s->fd = -1;
	s->can_read = 0;
	s->can_write = 0;
	s->read_hold = 0;
	s->write_quota = -1;
	s->read_watcher.fd = -1;
	s->write_watcher.fd = -1;
	s->read_dead = 1;
	s->write_dead = 1;
}

static void sock_shutdown_read(struct sock *s)
{
	s->can_read = 0;
}

static void sock_shutdown_write(struct sock *s)
{
	if (!s->can_write)
		return;

	s->can_write = 0;

	if (s->write_quota == 0 && fbuf_avail(&s->write_buf) > 0)
		return;

	ev_io_start(g_loop, &s->write_watcher);
	ev_feed_event(g_loop, &s->write_watcher, EV_WRITE);
}

void sock_shutdown(struct sock *s, int how)
{
	if (how & SOCK_SHUT_READ)
		sock_shutdown_read(s);

	if (how & SOCK_SHUT_WRITE)
		sock_shutdown_write(s);
}

static void sock_wake(struct sock *s)
{
	s->async = 1;
	ev_feed_event(g_loop, &s->read_watcher, EV_READ);
}

void sock_block(struct sock *s, int hold)
{
	if (s->read_dead)
		return;

	s->read_hold = !!hold;

	if (hold) {
		ev_io_stop(g_loop, &s->read_watcher);
		if (s->async)
			sock_wake(s);
		return;
	}

	ev_io_start(g_loop, &s->read_watcher);

	if (fbuf_avail(&s->read_buf) > 0)
		sock_wake(s);
}

void sock_quota(struct sock *s, ssize_t quota)
{
	if (s->write_dead)
		return;

	s->write_quota = quota;

	if (quota == 0) {
		ev_io_stop(g_loop, &s->write_watcher);
		return;
	}

	if (fbuf_avail(&s->write_buf) == 0)
		return;

	ev_io_start(g_loop, &s->write_watcher);
}

void sock_commit(struct sock *s)
{
	if (!s->can_write || s->write_quota == 0)
		return;

	if (fbuf_avail(&s->write_buf) == 0)
		return;

	ev_io_start(g_loop, &s->write_watcher);
}

static void sock_do_read(struct ev_loop *loop, struct ev_io *w, int revents)
{
	struct sock *s = w->data;
	ssize_t ret = 0;

	if (s->async) {
		s->func(s, SOCK_READ, 0);
		s->async = 0;
		return;
	}

	if (s->read_hold) {
		ev_io_stop(loop, w);
		return;
	}

	if (!s->can_read) {
read_shutdown:
		s->can_read = 0;
		s->read_dead = 1;
		ev_io_stop(loop, w);
		shutdown(w->fd, SHUT_RD);
		s->func(s, SOCK_SHUT_READ, 0);
		return;
	}

	if (revents & EV_ERROR)
		goto read_shutdown;

	if (!(revents & EV_READ))
		return;

	ret = fbuf_read(&s->read_buf, w->fd, IO_SIZE);
	if (ret == -EAGAIN || ret == -EWOULDBLOCK || ret == -EINTR)
		return;

	if (ret <= 0)
		goto read_shutdown;

	s->func(s, SOCK_READ, ret);
}

static void sock_do_write(struct ev_loop *loop, struct ev_io *w, int revents)
{
	struct sock *s = w->data;
	ssize_t ret, quota;

	if (!s->can_write && fbuf_avail(&s->write_buf) == 0) {
write_shutdown:
		s->can_write = 0;
		s->write_dead = 1;
		ev_io_stop(loop, w);
		shutdown(w->fd, SHUT_WR);
		s->func(s, SOCK_SHUT_WRITE, 0);
		return;
	}

	if (revents & EV_ERROR)
		goto write_shutdown;

	if (!(revents & EV_WRITE))
		return;

	quota = s->write_quota;
	if (quota < 0)
		quota = IO_SIZE;

	ret = fbuf_write(&s->write_buf, w->fd, quota);
	if (ret == -EAGAIN || ret == -EWOULDBLOCK || ret == -EINTR)
		return;

	if (ret < 0)
		goto write_shutdown;

	if (s->write_quota > 0)
		s->write_quota -= ret;

	if (fbuf_avail(&s->write_buf) == 0 || s->write_quota == 0)
		ev_io_stop(loop, w);

	if (ret == 0)
		return;

	s->func(s, SOCK_WRITE, ret);
}
