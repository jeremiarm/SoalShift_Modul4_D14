#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
static const char *dirpath = "/home/eplayer/Downloads";
static int e8_getattr(const char *path, struct stat *stbuf)
{
int res;
char fpath[1000];
sprintf(fpath,"%s%s",dirpath,path);
res = lstat(fpath, stbuf);
if (res == -1)
return -errno;
return 0;
}
static int e8_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
off_t offset, struct fuse_file_info *fi)
{
char fpath[1000];
if(strcmp(path,"/") == 0)
{
path=dirpath;
sprintf(fpath,"%s",path);
}
else sprintf(fpath, "%s%s",dirpath,path);
int res = 0;
DIR *dp;
struct dirent *de;
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
res = (filler(buf, de->d_name, &st, 0));
if(res!=0) break;
}
closedir(dp);
return 0;
}
static int e8_read(const char *path, char *buf, size_t size, off_t offset,
struct fuse_file_info *fi)
{
char fpath[1000];
if(strcmp(path,"/") == 0)
{
path=dirpath;
sprintf(fpath,"%s",path);
}
else sprintf(fpath, "%s%s",dirpath,path);
int res = 0;
int fd = 0 ;
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
static int e8_rename(const char *from, const char *to)
{
// juga digunakan untuk menyimpan / menyimpan hasil perubahan isi berkas
int res;
char ffrom[1000];
char tto[1000];
system("mkdir /home/eplayer/Downloads/simpanan -p");
char direk[] = "/home/eplayer/Downloads/simpanan";
sprintf(ffrom,"%s%s",dirpath,from);
sprintf(tto,"%s%s",direk,to);
res = rename(ffrom, tto);
char command[1000];
sprintf(command,"cp %s %s",ffrom,tto);
system(command);
if(res == -1)
return -errno;
return 0;
}
static int e8_write(const char *path, const char *buf, size_t size,
off_t offset, struct fuse_file_info *fi)
{
int fd;
int res;
char fpath[1000];
sprintf(fpath,"%s%s", dirpath, path);
(void) fi;
fd = open(fpath, O_WRONLY);
if (fd == -1)
return -errno;
res = pwrite(fd, buf, size, offset);
if (res == -1)
res = -errno;
close(fd);
return res;
}
static int e8_truncate(const char *path, off_t size)
{
int res;
char fpath[1000];
sprintf(fpath,"%s%s", dirpath, path);
res = truncate(fpath, size);
if(res == -1)
return -errno;
return 0;
}
static int e8_mknod(const char *path, mode_t mode, dev_t rdev)
{
int res;
char fpath[1000];
sprintf(fpath,"%s%s", dirpath, path);
res = mknod(fpath, mode, rdev);
if(res == -1)
return -errno;
return 0;
}
static int e8_open(const char *path, struct fuse_file_info *fi)
{
char fpath[1000];
if(strcmp(path,"/") == 0)
{
path=dirpath;
sprintf(fpath,"%s",path);
}
else sprintf(fpath, "%s%s",dirpath,path);
int res;
res = open(fpath, fi->flags);
if (res == -1)
return -errno;
close(res);
return 0;
}
static int e8_utimens(const char *path, const struct timespec ts[2])
{
int res;
/* don't use utime/utimes since they follow symlinks */
res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
if (res == -1)
return -errno;
return 0;
}
static struct fuse_operations e8_oper = {
.getattr	= e8_getattr,
.readdir	= e8_readdir,
.read	= e8_read,
.rename = e8_rename,
.write = e8_write,
.truncate	= e8_truncate,
.mknod	= e8_mknod,
.open	= e8_open,
.utimens = e8_utimens,
};
int main(int argc, char *argv[])
{
umask(0);
return fuse_main(argc, argv, &e8_oper, NULL);
}
