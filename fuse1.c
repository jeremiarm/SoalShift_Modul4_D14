#define FUSE_USE_VERSION 28
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
 
#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif 
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif 
#include<stdlib.h>
#include <sys/types.h>
 
static const char *dirpath = "/home/eplayer/Documents";


 
static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
 
	res = lstat(fpath, stbuf);
	if (res == -1)
	return -errno;
 
	return 0;
}
 

static int xmp_readdir(const char *path, void *buf,
fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
 
	(void) offset;
	(void) fi;
 
	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;
 
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
		break;
	}
 
	closedir(dp);
	return 0;
}
 

 
static int xmp_read(const char *path, char *buf,
size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	char check[1000];
        char target[1000];
        sprintf(fpath,"%s%s",dirpath,path);
        int jarak=strlen(fpath);
        strcpy(check,fpath+jarak-4);

        if(strcmp(check,".txt")==0 || strcmp(check,".pdf")==0||strcmp(check,".doc")==0)
        {
	sprintf(target,"%s.ditandai",fpath);
        res=rename(fpath,target);
       system("zenity --error --text=\"Terjadi Kesalahan! File berisik konten berbahaya.\n\" --title=\"Peringatan\"" );
                return -errno;
        }

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;
 
	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
 
	close(fd);
	return res;
}
 
static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
};
 
int main(int argc, char *argv[])
{
	fuse_main(argc, argv, &xmp_oper, NULL);
	return 0;
}
