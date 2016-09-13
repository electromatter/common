#ifndef NET_H
#define NET_H

struct fbuf;

#include <netinet/in.h>

/*TODO: namespace net_*/

int make_listener(const struct sockaddr *addr, socklen_t len);
int make_connection(const struct sockaddr *addr, socklen_t len);
int accept_connection(int listener, struct sockaddr *addr, socklen_t *len);
int set_nonblock(int fd, int nonblock);
int set_cloexec(int fd, int cloexec);
int set_nodelay(int fd, int nodelay);
int set_cork(int fd, int corked);

ssize_t fbuf_read(struct fbuf *buf, int fd, ssize_t size);
ssize_t fbuf_write(struct fbuf *buf, int fd, ssize_t size);

int net_format_addr(char *dest, size_t size, struct sockaddr *addr, socklen_t addrsize);
int net_peer_addr(char *dest, size_t size, int fd);

#endif
