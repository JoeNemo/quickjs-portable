#ifndef  __CLANGBETA2_POLYFILL__
#define __CLANGBETA2_POLYFILL__

#if __CHARSET_LIB == 1
#define QASCII 1
#endif

#include <time.h>

size_t malloc_usable_size (const void *ptr);

int32_t atomicIncrementI32(int32_t *intPointer, int32_t increment);
int64_t atomicIncrementI64(int64_t *intPointer, int64_t increment);

typedef int clockid_t;

#define CLOCK_REALTIME  0
#define CLOCK_MONOTONIC 1

int clock_gettime(clockid_t clk_id, struct timespec *tp);

/*
   struct tm is short a few fields
 
   The glibc version of struct tm has additional fields

    long tm_gmtoff;           -- Seconds east of UTC 
    const char *tm_zone;      -- Timezone abbreviation 


 */

void dumpbuffer(void *buffer, int length); /* temp hack */

int convertOpenStream(int fd, unsigned short fileCCSID);

#define CHARSET_ISO8859 819

int tagFile(char *pathname, unsigned short ccsid);


#endif
