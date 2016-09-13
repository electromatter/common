#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

int daemonize(void)
{
	/* TODO */
	/* fork off */
	/* set umask */
	/* get a sid */
	/* redirect stdin, stdout, stderr */
	/* close all open files */
	/* obtain pid lock file */
	/* open control listener */
	/* chdir to root or some chroot */
	/* drop uid=0 by setuid/setgid */
	/* drop non-essential permissions */
	/* drop non-essential syscalls */
	return 0;
}

int resolve_uidgid(const char *uidgid, int *uid, int *gid)
{
	struct passwd *pwd = NULL;
	struct group *grp = NULL;
	char uname[256] = "";
	char *gname = NULL;

	strcpy(uname, uidgid);
	gname = strchr(uname, ':');
	if (gname != NULL)
		*gname++ = 0;

	if (sscanf(uname, "%d", uid) < 1) {
		pwd = getpwnam(uname);
		if (pwd == NULL)
			return -1;
		*uid = pwd->pw_uid;
		*gid = pwd->pw_gid;
	}

	if (gname == NULL) {
		if (pwd == NULL)
			*gid = *uid;
		return 0;
	}

	if (sscanf(gname, "%d", gid) < 1) {
		grp = getgrnam(gname);
		if (grp == NULL)
			return -1;
		*gid = grp->gr_gid;
	}

	return 0;
}

int droproot(uid_t uid, gid_t gid)
{
	uid_t olduid = getuid();
	gid_t oldgid = getgid();

	/* tried to droproot into root? */
	if (uid == 0 || gid == 0)
		return -1;

	/* set uid/gid */
	if (setegid(gid) < 0 || seteuid(uid) < 0
			|| setgid(gid) < 0 || setuid(uid) < 0)
		return -1;

	/* check we cannot change back */
	if (gid != oldgid && (setegid(oldgid) >= 0 || setgid(oldgid) >= 0))
		return -1;

	if (uid != olduid && (seteuid(olduid) >= 0 || setuid(olduid) >= 0))
		return -1;

	return 0;
}

int pidlock(const char *lockpath)
{
	char text[32];
	pid_t pid = getpid();
	int fd;

	fd = open(lockpath, O_RDWR | O_CLOEXEC | O_CREAT,
			S_IRUSR | S_IWUSR);
	if (fd < 0)
		goto err_ret;

	if (lockf(fd, F_TLOCK, 0) < 0)
		goto err_fd;

	if (ftruncate(fd, 0) < 0)
		goto err_fd;

	sprintf(text, "%i\n", pid);
	if (write(fd, text, strlen(text)) < 0)
		goto err_fd;

	return fd;

err_fd:
	close(fd);
err_ret:
	if (errno >= 0)
		return -1;
	return -errno;
}
