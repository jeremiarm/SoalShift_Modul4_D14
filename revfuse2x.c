#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/ilmi/Downloads";
static const char *savepath = "/home/ilmi/Downloads/simpanan";
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

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
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

static int xmp_read(const char *path, char *buf,
size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",savepath,path);
	char check[1000];
        int jarak=strlen(fpath);
        strcpy(check,fpath+jarak-5);

        if(strcmp(check,".copy")==0)
        {
system("zenity --error --text=\"File yang anda buka adalah hasil file salinan. File tidak bisa diubah maupun disalin kembali\n\" --title=\"Peringatan\"" );
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

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char result[1000]; 
	char asal[1000];
	system("mkdir /home/ilmi/Downloads/simpanan -p");
	sprintf(result,"%s%s.copy",savepath,to);
	sprintf(asal,"%s%s",dirpath,from);
	res = rename(asal, result);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = mknod(fpath, mode, rdev);
	if (res == -1)
	return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	if(strcmp(path,"/")==0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath,"%s%s",dirpath,path);
	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;
	close(res);
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = truncate(fpath, size);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	//char fpath[1000];
	//sprintf(fpath,"%s%s",dirpath,path);
	res = utimensat(0,path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	(void) fi;
	sprintf(fpath,"%s%s",dirpath,path);
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;
	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
	close(fd);
	//printf("pathnya jadi %s\n",fpath);
	return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.write		= xmp_write,
	.rename		= xmp_rename,
	.mknod		= xmp_mknod,
	.open		= xmp_open,
	.chmod		= xmp_chmod,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

