#ifndef __WINDIRENT__ 
#define __WINDIRENT__  1

#include <windows.h>
#include <stdbool.h>
#include <sys/types.h>
/* #include <winsock2.h> */

#define NAME_MAX 255


struct dirent {
  ino_t d_ino;       /* inode number */
  char d_name[NAME_MAX+1]; /* filename */
};



typedef struct DIR_tag {
  HANDLE hFind;
  WIN32_FIND_DATA findData;
  bool   hasMoreEntries;
  struct dirent entry;
} DIR;

DIR *opendir(const char *);

struct dirent *readdir(DIR *);

void rewinddir(DIR *);

int closedir(DIR *);



#endif 
