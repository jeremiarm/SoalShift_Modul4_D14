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


static struct fuse_operations xmp_oper = {
.getattr	= xmp_getattr,
.write		= xmp_write,
.readlink	= xmp_readlink,
.readdir	= xmp_readdir,
.mknod		= xmp_mknod,
.rename		= xmp_rename,
.chmod		= xmp_chmod,
.truncate	= xmp_truncate,
.utimens	= xmp_utimens,
.open		= xmp_open,
};
