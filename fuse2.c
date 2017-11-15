#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef linux
#define _XOPEN_SOURCE 500
#endif
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif
