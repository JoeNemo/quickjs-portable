#ifndef __WINSTDIO_EXT__ 
#define __WINSTDIO_EXT__  1

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
/* #include <winsock2.h> */

#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>

/* Good info at https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/read?view=msvc-170 */

typedef int64_t ssize_t; /* I don't know where this comes from, but it's necessary */

/* Linux-y STAT_H stuff */

/* #define S_IFMT  00170000  window MSVC has this */
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
/* #define S_IFREG  0100000   window MSVC has this */
#define S_IFBLK  0060000
/* #define S_IFDIR  0040000   windows MSVC has this */
/* #define S_IFCHR  0020000   windows MSVC has this */
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

typedef uint32_t mode_t;

/* Hacky, but */
#define getcwd(b, maxlen) _getcwd(b,maxlen)
#define chdir(p) _chdir(p)
#define rmdir(p) _rmdir(p)
#define mkdir(p) _mkdir(p)

/* Many of these are really part of unistd */
/*
int open(const char *path, int oflag, ... );
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
ssize_t write(int fd, const void *buf, size_t nbyte);
ssize_t read(int fd, void *buf, size_t nbyte);
*/

/*
int chdir(const char *path);
int rmdir(const char *path);
int mkdir(const char *path, mode_t mode);
*/

int isatty(int fd);

/* FILE	*fdopen(int, const char *);*/
/* int	 fileno(FILE *);*/

/* 
int	 pclose(FILE *);
FILE	*popen(const char *, const char *);
*/

#define popen(c,t) _popen(c,t)
#define pclose(s) _pclose(s)

#define open(file,flags,mode) _open(file,flags,mode)
#define close(fd) _close(fd)
#define fdopen(fd,mode) _fdopen(fd,mode)
#define fileno(file) _fileno(file)
#define isatty(fd) _isatty(fd)
#define unlink(f) _unlink(f)
#define write(fd,buf,len) _write(fd,buf,len)
#define read(fd,buf,len) _read(fd,buf,len)
#define lseek(fd, pos, whence) _lseek(fd, pos, whence)

#endif 
