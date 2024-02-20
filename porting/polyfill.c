#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "polyfill.h"

size_t malloc_usable_size (const void *ptr){
  char *data = (char*)ptr;
  int64_t address =  (int64_t)ptr;
  if (address > 0x100000000l){
    data -= 4;
    return *((int*)data) - 0x10; /* that's the 64 bit malloc overhead */
  } else{
    data -= 4;
#ifdef _LP64
    return *((int*)data) - 0x10; /* that's the 31 bit malloc overhead */
#else
    return *((int*)data) - 0x08; /* that's the 31 bit malloc overhead */
#endif
  }
}

#if defined(__XPLINK__)
int32_t atomicIncrementI32(int32_t *place, int32_t increment){
  int32_t oldValue = 0;
  int32_t newValue = 0;  /* just to let compiler figure out a register */
  int pswMask = 0;
#ifdef _LP64
  int64_t addr = (int64_t)place;
#else 
  int32_t addr = (int32_t)place;
#endif
  while (1){
    oldValue = *place;
    newValue = oldValue+increment;
    /* printf("old=%d new=%d\n",oldValue,newValue); */
    __asm("         L  9,%0   \n"
          "         L  10,%1   \n"
	  "         LG 11,%2  \n"
	  "         CS 9,10,0(11) \n"
	  "         EPSW 10,0 note re-use of r10 \n"
	  "         ST 10,%3 "
	  :
	  :
	  "m"(oldValue),"m"(newValue),"m"(addr),"m"(pswMask)
	  :
	  "r9","r10","r11");
    int cc = 0x3000 & pswMask;
    /* printf("pswMask=0x%08X, old=%d\n",pswMask,oldValue); */
    if (cc == 0){ /* Pseudo JZ out of loop */
      break;
    }
  }
  return oldValue;
}

#else 

int32_t atomicIncrementI32(int32_t *place, int32_t increment){
  int old = *place;
  /* printf("Ay caramba\n"); */
  *place = (old + increment);
  return old;
}

#endif


/*
struct timespec {
        time_t   tv_sec;        -- seconds 
        long     tv_nsec;       -- nanoseconds 
};
*/

#define STCK_MICRO_SHIFT 12
static const uint64_t unixSecondsAfterZ = UINT64_C(2208988800);

/* this ain't great, but it's better than nothing */

int clock_gettime(clockid_t clk_id, struct timespec *tp){
  uint64_t stck;
#ifdef _LP64
  __asm(" STCK %0 "
	: : "m"(stck): "r15");
  long micros = (long)(stck&0xFFF);
  time_t unixSeconds = (time_t)((stck >> STCK_MICRO_SHIFT) - unixSecondsAfterZ);

  switch (clk_id){
  case CLOCK_REALTIME:
  case CLOCK_MONOTONIC:
    tp->tv_sec = unixSeconds;
    tp->tv_nsec = (micros * 1000);
    return 0;
  default:
    {
      printf("*** PANIC *** unhandled clockid_t=%d\n",clk_id);
      return EINVAL; /* 22 */
    }
  }
#else
  return 0; /* fix me */
#endif
}

int convertOpenStream(int fd, unsigned short fileCCSID){
  struct f_cnvrt conversionArg;
  conversionArg.cvtcmd = SETCVTON; /* SETCVTOFF; */
  conversionArg.pccsid = 0;
  conversionArg.fccsid = fileCCSID; /* 1047; */
  int res = fcntl(fd, F_CONTROL_CVT, &conversionArg);
  if (res != 0){
    printf("* internal error* convertOpenStream(), and ascii/ebcdic function, called fcntl failed errno=%d\n",errno);
  }
  return res;
}

#define CCSID_BINARY 65535
#define CCSID_NONE 0

int tagFile(const char *pathname, unsigned short ccsid){
#if defined(_LP64) && defined(ZCOMPILE_CLANG)
  attrib64_t attr;
  memset(&attr,0,sizeof(attrib64_t));

  attr.att_filetagchg = 1;
  attr.att_filetag.ft_ccsid = ccsid;
  if (ccsid == CCSID_NONE || ccsid == CCSID_BINARY ) {
    attr.att_filetag.ft_txtflag = 0;
  } else {
    attr.att_filetag.ft_txtflag = 1;
  }

  int res = __chattr64((char*)pathname, &attr, sizeof(attr));
#else
  attrib_t attr;
  memset(&attr,0,sizeof(attrib_t));

  attr.att_filetagchg = 1;
  attr.att_filetag.ft_ccsid = ccsid;
  if (ccsid == CCSID_NONE || ccsid == CCSID_BINARY ) {
    attr.att_filetag.ft_txtflag = 0;
  } else {
    attr.att_filetag.ft_txtflag = 1;
  }

  int res = __chattr((char*)pathname, &attr, sizeof(attr));
#endif
  if (res){
    printf("chattr failed with errno=%d\n",errno);
  }
  return res;
}

#define EXTATTR_SHARELIB 0x10
#define EXTATTR_NO_SHAREAS 0x08
#define EXTATTR_APF_AUTH 0x04
#define EXTATTR_PROGCTL 0x02

int changeExtendedAttributes(const char *pathname, int attribute, bool onOff){
#if defined(_LP64) && defined(ZCOMPILE_CLANG)
  attrib64_t attr;
  memset(&attr,0,sizeof(attrib64_t));

  attr.att_setgen = 1;
  switch (attribute){
  case EXTATTR_SHARELIB:
    attr.att_sharelibmask = 1;
    attr.att_sharelib = (onOff ? 1 : 0);
    break;
  case EXTATTR_NO_SHAREAS:
    attr.att_noshareasmask = 1;
    attr.att_noshareas = (onOff ? 1 : 0);
    break;
  case EXTATTR_APF_AUTH:
    attr.att_apfauthmask = 1;
    attr.att_apfauth = (onOff ? 1 : 0);
    break;
  case EXTATTR_PROGCTL:
    attr.att_progctlmask = 1;
    attr.att_progctl = (onOff ? 1 : 0);
    break;
  }

  int res = __chattr64((char*)pathname, &attr, sizeof(attr));
#else
  attrib_t attr;
  memset(&attr,0,sizeof(attrib_t));

  attr.att_setgen = 1;
  switch (attribute){
  case EXTATTR_SHARELIB:
    attr.att_sharelibmask = 1;
    attr.att_sharelib = (onOff ? 1 : 0);
    break;
  case EXTATTR_NO_SHAREAS:
    attr.att_noshareasmask = 1;
    attr.att_noshareas = (onOff ? 1 : 0);
    break;
  case EXTATTR_APF_AUTH:
    attr.att_apfauthmask = 1;
    attr.att_apfauth = (onOff ? 1 : 0);
    break;
  case EXTATTR_PROGCTL:
    attr.att_progctlmask = 1;
    attr.att_progctl = (onOff ? 1 : 0);
    break;
  }

  int res = __chattr((char*)pathname, &attr, sizeof(attr));
#endif

  return res;
}

/* ADD */
int atomicAddInt(int *p, int x){
  int old = *p;
  *p = old+x;
  return old;
}

uint64_t atomicAddU64(_Atomic(uint64_t) *p, uint64_t x){
  uint64_t old = *p;
  *p = old+x;
  return old;
}

uint32_t atomicAddU32(_Atomic(uint32_t) *p, uint32_t x){
  uint32_t old = *p;
  *p = old+x;
  return old;
}

uint16_t atomicAddU16(_Atomic(uint16_t) *p, uint16_t x){
  uint16_t old = *p;
  *p = old+x;
  return old;
}

uint8_t atomicAddU8(_Atomic(uint8_t) *p, uint8_t x){
  uint8_t old = *p;
  *p = old+x;
  return old;
}

/* SUB */
int atomicSubInt(int *p, int x){
  int old = *p;
  *p = old-x;
  return old;
}

uint64_t atomicSubU64(_Atomic(uint64_t) *p, uint64_t x){
  uint64_t old = *p;
  *p = old-x;
  return old;
}

uint32_t atomicSubU32(_Atomic(uint32_t) *p, uint32_t x){
  uint32_t old = *p;
  *p = old-x;
  return old;
}

uint16_t atomicSubU16(_Atomic(uint16_t) *p, uint16_t x){
  uint16_t old = *p;
  *p = old-x;
  return old;
}

uint8_t atomicSubU8(_Atomic(uint8_t) *p, uint8_t x){
  uint8_t old = *p;
  *p = old-x;
  return old;
}

/* AND */
int atomicAndInt(int *p, int x){
  int old = *p;
  *p = old & x;
  return old;
}

uint64_t atomicAndU64(_Atomic(uint64_t) *p, uint64_t x){
  uint64_t old = *p;
  *p = old & x;
  return old;
}

uint32_t atomicAndU32(_Atomic(uint32_t) *p, uint32_t x){
  uint32_t old = *p;
  *p = old & x;
  return old;
}

uint16_t atomicAndU16(_Atomic(uint16_t) *p, uint16_t x){
  uint16_t old = *p;
  *p = old & x;
  return old;
}

uint8_t atomicAndU8(_Atomic(uint8_t) *p, uint8_t x){
  uint8_t old = *p;
  *p = old & x;
  return old;
}

/* OR */
int atomicOrInt(int *p, int x){
  int old = *p;
  *p = old | x;
  return old;
}

uint64_t atomicOrU64(_Atomic(uint64_t) *p, uint64_t x){
  uint64_t old = *p;
  *p = old | x;
  return old;
}

uint32_t atomicOrU32(_Atomic(uint32_t) *p, uint32_t x){
  uint32_t old = *p;
  *p = old | x;
  return old;
}

uint16_t atomicOrU16(_Atomic(uint16_t) *p, uint16_t x){
  uint16_t old = *p;
  *p = old | x;
  return old;
}

uint8_t atomicOrU8(_Atomic(uint8_t) *p, uint8_t x){
  uint8_t old = *p;
  *p = old | x;
  return old;
}

/* XOR */
int atomicXorInt(int *p, int x){
  int old = *p;
  *p = old ^ x;
  return old;
}

uint64_t atomicXorU64(_Atomic(uint64_t) *p, uint64_t x){
  uint64_t old = *p;
  *p = old ^ x;
  return old;
}

uint32_t atomicXorU32(_Atomic(uint32_t) *p, uint32_t x){
  uint32_t old = *p;
  *p = old ^ x;
  return old;
}

uint16_t atomicXorU16(_Atomic(uint16_t) *p, uint16_t x){
  uint16_t old = *p;
  *p = old ^ x;
  return old;
}

uint8_t atomicXorU8(_Atomic(uint8_t) *p, uint8_t x){
  uint8_t old = *p;
  *p = old ^ x;
  return old;
}

/* LOAD */
int atomicLoadInt(int *p){
  int old = *p;
  return old;
}

uint64_t atomicLoadU64(_Atomic(uint64_t) *p){
  uint64_t old = *p;
  return old;
}

uint32_t atomicLoadU32(_Atomic(uint32_t) *p){
  uint32_t old = *p;
  return old;
}

uint16_t atomicLoadU16(_Atomic(uint16_t) *p){
  uint16_t old = *p;
  return old;
}

uint8_t atomicLoadU8(_Atomic(uint8_t) *p){
  uint8_t old = *p;
  return old;
}

/* STORE */
void atomicStoreInt(int *p, int x){
  *p = x;
}

void atomicStoreU64(_Atomic(uint64_t) *p, uint64_t x){
  *p = x;
}

void atomicStoreU32(_Atomic(uint32_t) *p, uint32_t x){
  *p = x;
}

void atomicStoreU16(_Atomic(uint16_t) *p, uint16_t x){
  *p = x;
}

void atomicStoreU8(_Atomic(uint8_t) *p, uint8_t x){
  *p = x;
}

/* Exchange */
int atomicExchangeInt(int *p, int x){
  int old = *p;
  *p = x;
  return old;
}

uint64_t atomicExchangeU64(_Atomic(uint64_t) *p, uint64_t x){
  uint64_t old = *p;
  *p = x;
  return old;
}

uint32_t atomicExchangeU32(_Atomic(uint32_t) *p, uint32_t x){
  uint32_t old = *p;
  *p = x;
  return old;
}

uint16_t atomicExchangeU16(_Atomic(uint16_t) *p, uint16_t x){
  uint16_t old = *p;
  *p = x;
  return old;
}

uint8_t atomicExchangeU8(_Atomic(uint8_t) *p, uint8_t x){
  uint8_t old = *p;
  *p = x;
  return old;
}

/* Compare_Exchange_Strong */
_Bool atomicCompareExchangeStrongInt(int *p, int *exp, int desired){
  if (*p == *exp){
    *p = desired;
    return true;
  } else{
    *exp = *p;
    return false;
  } 
}

_Bool atomicCompareExchangeStrongU64(_Atomic(uint64_t) *p, uint64_t *exp, uint64_t desired){
  if (*p == *exp){
    *p = desired;
    return true;
  } else{
    *exp = *p;
    return false;
  } 
}

_Bool atomicCompareExchangeStrongU32(_Atomic(uint32_t) *p, uint32_t *exp, uint32_t desired){
  if (*p == *exp){
    *p = desired;
    return true;
  } else{
    *exp = *p;
    return false;
  } 
}

_Bool atomicCompareExchangeStrongU16(_Atomic(uint16_t) *p, uint16_t *exp, uint16_t desired){
  if (*p == *exp){
    *p = desired;
    return true;
  } else{
    *exp = *p;
    return false;
  } 
}

_Bool atomicCompareExchangeStrongU8(_Atomic(uint8_t) *p, uint8_t *exp, uint8_t desired){
  if (*p == *exp){
    *p = desired;
    return true;
  } else{
    *exp = *p;
    return false;
  } 
}


