#include <limits.h>

#include <sys/time.h>

#include <common/util.h>

#define MSEC_MAX	(INT_MAX)

void timespec_neg(struct timespec *acc)
{
	acc->tv_sec += 1;
	acc->tv_sec *= -1;
}

void timespec_sub(struct timespec *acc, const struct timespec *delta)
{
	acc->tv_sec -= delta->tv_sec;
	acc->tv_nsec -= delta->tv_nsec;
	if (acc->tv_nsec < 0) {
		acc->tv_nsec += 1000000000;
		acc->tv_sec -= 1;
	}
}

int timespec_cmp(const struct timespec *left, const struct timespec *right)
{
	if (left->tv_sec > right->tv_sec)
		return 1;
	if (left->tv_sec < right->tv_sec)
		return -1;
	if (left->tv_nsec > right->tv_nsec)
		return 1;
	if (left->tv_nsec < right->tv_nsec)
		return -1;
	return 0;
}

void timespec_add(struct timespec *acc, const struct timespec *delta)
{
	acc->tv_sec += delta->tv_sec;
	acc->tv_nsec += delta->tv_nsec;
	if (acc->tv_nsec >= 1000000000) {
		acc->tv_nsec -= 1000000000;
		acc->tv_sec += 1;
	}
}

void timespec_mul(struct timespec *acc, const struct timespec *delta)
{
	struct timespec temp = *acc;
	acc->tv_nsec = (temp.tv_nsec * delta->tv_nsec) / 1000000000;
	acc->tv_nsec += (temp.tv_sec % 1000000000) * delta->tv_nsec;
	acc->tv_nsec += (delta->tv_sec % 1000000000) * temp.tv_nsec;
	acc->tv_sec = acc->tv_nsec / 1000000000;
	acc->tv_nsec %= 1000000000;
	acc->tv_sec += (temp.tv_sec / 1000000000) * delta->tv_nsec;
	acc->tv_sec += (delta->tv_sec / 1000000000) * temp.tv_nsec;
	acc->tv_sec += temp.tv_sec * delta->tv_sec;
}

int timespec_to_msec(const struct timespec *x)
{
	if (x->tv_sec > MSEC_MAX / 1000)
		return MSEC_MAX;
	if (x->tv_sec < -MSEC_MAX / 1000)
		return -MSEC_MAX;
	return x->tv_sec * 1000 + x->tv_nsec / 1000000;
}

double timespec_to_sec(const struct timespec *x)
{
	return x->tv_sec + x->tv_nsec * 1e-9;
}
