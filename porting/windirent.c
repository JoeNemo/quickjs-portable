#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "windirent.h"

DIR *opendir(const char *directoryName){
  int nameLength = strlen(directoryName);
  WIN32_FIND_DATA findData;
  int directoryNameBufferSize = nameLength + 10;
  char *directoryNameBuffer = malloc(directoryNameBufferSize);
  HANDLE hFind = INVALID_HANDLE_VALUE;

  int len = sprintf(directoryNameBuffer,"%s\\*",directoryName);
  hFind = FindFirstFile(directoryNameBuffer, &findData);

  free(directoryNameBuffer);
  directoryNameBuffer = NULL;
 
  if (hFind == INVALID_HANDLE_VALUE){
    errno = GetLastError();
    return NULL;
  } else {
    DIR *dir = (DIR*)malloc(sizeof(DIR));
    memset(dir,0,sizeof(DIR));
    dir->hFind = hFind;
    memcpy(&(dir->findData),&findData,sizeof(WIN32_FIND_DATA));
    dir->hasMoreEntries = TRUE;

    return dir;
  }
}

struct dirent *readdir(DIR *dir){
  HANDLE hFind = dir->hFind;
  WIN32_FIND_DATA *findData = &(dir->findData);
  
  if (dir->hasMoreEntries){
    int filenameLength = strlen(findData->cFileName);
    struct dirent *entry = &dir->entry;
    entry->d_ino = 0;
    memcpy(entry->d_name,findData->cFileName,filenameLength);
    if (FindNextFile(hFind, findData)){
      dir->hasMoreEntries = TRUE;
    } else {
      /* find loop is done */
      dir->hasMoreEntries = FALSE;
    }
    return entry;
  } else {
    return NULL;
  }
}

int closedir(DIR *dir){
  if (dir){
    HANDLE hFind = dir->hFind;
    FindClose(hFind);
    free(dir);
    return 0;
  } else {
    errno = -1;
    return -1;
  }
}
