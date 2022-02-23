#include <stdio.h>
#include "wintime.h"
#include <winsock.h>


#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
 
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;
 
  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);
 
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
 
    /*converting file time to unix epoch*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS; 
    tmpres /= 10;  /*convert into microseconds*/
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }
 
  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }
 
  return 0;
}

static LARGE_INTEGER ticksPerSecond = { .QuadPart = 0 };

static int initMonotonic(){
    QueryPerformanceFrequency(&ticksPerSecond);
    if (!ticksPerSecond.QuadPart) {
      errno = ENOTSUP;
      return -1;
    } else {
      return 0;
    }
  }
  

int clock_gettime(clockid_t id, struct timespec *tp){
  switch (id){
  case CLOCK_REALTIME:
    {
      if (timespec_get(tp, TIME_UTC) == TIME_UTC){
        return 0;
      } else {
        return EINVAL;
      }
    }
  case CLOCK_MONOTONIC:
    {
      LARGE_INTEGER ticks;
      double seconds;
      if (!ticksPerSecond.QuadPart) {
        int initStatus = initMonotonic();
        if (initStatus){
          return -1;
        }
      }
      QueryPerformanceCounter(&ticks);
      
      seconds = (double) ticks.QuadPart / (double) ticksPerSecond.QuadPart;
      tp->tv_sec = (time_t)seconds;
      tp->tv_nsec = (long)((ULONGLONG)(seconds * 1000000000) % 1000000000);

      return 0;
    }
  default:
    {
      printf("*** PANIC *** unhandled clockid_t=%d\n",id);
      return EINVAL; /* 22 */
    }
  }
}
 

/*
#define CLOCK_REALTIME			0
#define CLOCK_MONOTONIC			1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID		3
#define CLOCK_MONOTONIC_RAW		4
#define CLOCK_REALTIME_COARSE		5
#define CLOCK_MONOTONIC_COARSE		6
#define CLOCK_BOOTTIME			7
#define CLOCK_REALTIME_ALARM		8
#define CLOCK_BOOTTIME_ALARM		9

typedef long clockid_t;

int clock_getres(clockid_t clk_id, struct timespec *res);
int clock_gettime(clockid_t clk_id, struct timespec *tp);
int clock_settime(clockid_t clk_id, const struct timespec *tp); 
*/
