#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <mcp_base/fbuf.h>

#include <common/net.h>

#define DEFAULT_SIZE		(64*1024)

int make_listener(const struct sockaddr *addr, socklen_t len)
{
	int fd, ret, err;
	const int one = 1;

	fd = socket(addr->sa_family, SOCK_STREAM, 0);
	if (fd < 0)
		return -errno;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
		goto err_fd;

	if (bind(fd, addr, len) < 0)
		goto err_fd;

	if (listen(fd, SOMAXCONN) < 0)
		goto err_fd;

	ret = set_nonblock(fd, 1);
	if (ret < 0) {
		close(fd);
		return ret;
	}

	return fd;

err_fd:
	err = errno;
	close(fd);
	return -err;
}

int make_connection(const struct sockaddr *addr, socklen_t len)
{
	int fd, err;

	fd = socket(addr->sa_family, SOCK_STREAM, 0);
	if (fd < 0)
		return -errno;

	/* FIXME */
	if (connect(fd, addr, len) < 0 && errno != EINPROGRESS)
		goto err_fd;

	if (set_nonblock(fd, 1) < 0)
		goto err_fd;
	
	return fd;

err_fd:
	err = errno;
	close(fd);
	return -err;
}

int accept_connection(int listener, struct sockaddr *addr, socklen_t *len)
{
	struct sockaddr_storage saddr;
	socklen_t slen = sizeof(saddr);
	int fd, ret;

	if (addr == NULL) {
		addr = (void*)&saddr;
		len = &slen;
	}

	memset(addr, 0, *len);
	fd = accept(listener, addr, len);
	if (fd < 0) {
		*len = 0;
		return -errno;
	}

	ret = set_nonblock(fd, 1);
	if (ret < 0) {
		close(fd);
		return ret;
	}

	return fd;
}

int set_nonblock(int fd, int nonblock)
{
	int flags = fcntl(fd, F_GETFL, 0);

	if (flags < 0)
		return -errno;

	flags &= ~O_NONBLOCK;
	if (nonblock)
		flags |= O_NONBLOCK;

	if (fcntl(fd, F_SETFL, flags))
		return -errno;

	return 0;
}

int set_cloexec(int fd, int cloexec)
{
	int flags = fcntl(fd, F_GETFD, 0);

	if (flags < 0)
		return -errno;

	flags &= ~FD_CLOEXEC;
	if (cloexec)
		flags |= FD_CLOEXEC;

	if (fcntl(fd, F_SETFD, flags))
		return -errno;

	return 0;
}

int set_nodelay(int fd, int nodelay)
{
	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) < 0)
		return -errno;

	return 0;
}

int set_cork(int fd, int corked)
{
	if (setsockopt(fd, IPPROTO_TCP, TCP_CORK, &corked, sizeof(corked)) < 0)
		return -errno;

	return 0;
}

ssize_t fbuf_read(struct fbuf *buf, int fd, ssize_t size)
{
	void *ptr = NULL;
	ssize_t ret;

	/* default */
	if (size < 0)
		size = DEFAULT_SIZE;

	/* limit to max wavail */
	if ((size_t)size > fbuf_max_wavail(buf))
		size = fbuf_max_wavail(buf);

	/* either alloc more or not */
	while (ptr == NULL) {
		if (size == 0)
			return 0;

		ptr = fbuf_wptr(buf, size);

		if (ptr == NULL)
			size = fbuf_wavail(buf);
	}

	/* preform the read */
	ret = read(fd, ptr, size);
	if (ret == 0)
		return 0;

	if (ret < 0)
		return -errno;

	/* report bytes read */
	fbuf_produce(buf, ret);
	return ret;
}

ssize_t fbuf_write(struct fbuf *buf, int fd, ssize_t size)
{
	ssize_t ret;

	/* limit and default to max avail */
	if (size < 0 || (size_t)size > fbuf_avail(buf))
		size = fbuf_avail(buf);

	if (size == 0)
		return 0;

	/* preform the write */
	ret = write(fd, fbuf_ptr(buf), size);
	if (ret == 0)
		return 0;

	if (ret < 0)
		return -errno;

	/* report bytes written */
	fbuf_consume(buf, ret);
	return ret;
}

/* TODO: more protocols */
int net_format_addr(char *dest, size_t size, struct sockaddr *addr, socklen_t addrsize)
{
	struct sockaddr_in *in = (void*)addr;

	switch (addr->sa_family) {
	case AF_INET:
		if (addrsize < sizeof(*in))
			return -EINVAL;
		if (inet_ntop(AF_INET, &in->sin_addr, dest, size) == NULL)
			return -errno;
		return 0;

	default:
		return -EINVAL;
	}
}

int net_peer_addr(char *dest, size_t size, int fd)
{
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);

	if (getpeername(fd, (void*)&addr, &len) < 0)
		return -errno;

	return net_format_addr(dest, size, (void*)&addr, len);
}
