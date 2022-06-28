#ifndef  __CLANG_POLYFILL__
#define __CLANG_POLYFILL__

#include <stdint.h>

#if defined(__GNUC__) && defined(__clang__)
#ifdef __IBMC__
#define ZCOMPILE_XLCLANG
#else 
#define ZCOMPILE_CLANG
#endif
#elif defined(__IBMC__)
#define ZCOMPILE_XLC
#else 
#error Unknown_ZOS_C_Compiler
#endif

#ifdef ZCOMPILE_XLCLANG 

/* Notation from standard

      http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf

   A - an atomic type
   C - corresponding non-atomic
   M - other arg for arithmetic, for atomic integer types, M is C 

  atomic_fetch_add( volatile void* obj, memory_order mo );

 The functions not ending in _explicit have the same semantics as the
corresponding _explicit function with memory_order_seq_cst for the
memory_order argument.

*/

enum memory_order{
  memory_order_relaxed = __ATOMIC_RELAXED,
  memory_order_consume = __ATOMIC_CONSUME,
  memory_order_acquire = __ATOMIC_ACQUIRE,
  memory_order_release = __ATOMIC_RELEASE,
  memory_order_acq_rel = __ATOMIC_ACQ_REL,
  memory_order_seq_cst = __ATOMIC_SEQ_CST,
};

typedef enum memory_order memory_order;

typedef _Atomic(int) atomic_int;
typedef _Atomic(uint8_t) atomic_uint8_t;
typedef _Atomic(uint16_t) atomic_uint16_t;
typedef _Atomic(uint32_t) atomic_uint32_t;
typedef _Atomic(uint64_t) atomic_uint64_t;

#ifdef _LP64
#define atomic_fetch_add(ptr, val) __c11_atomic_fetch_add(ptr, val, memory_order_seq_cst)
#define atomic_fetch_sub(ptr, val) __c11_atomic_fetch_sub(ptr, val, memory_order_seq_cst)
#define atomic_fetch_and(ptr, val) __c11_atomic_fetch_and(ptr, val, memory_order_seq_cst)
#define atomic_fetch_or(ptr, val) __c11_atomic_fetch_or(ptr, val, memory_order_seq_cst)
#define atomic_fetch_xor(ptr, val) __c11_atomic_fetch_xor(ptr, val, memory_order_seq_cst)
#define atomic_load(ptr) __c11_atomic_load(ptr, memory_order_seq_cst)
#define atomic_store(ptr, val) __c11_atomic_store(ptr, val, memory_order_seq_cst)
#define atomic_exchange(ptr, val) __c11_atomic_exchange(ptr, val, memory_order_seq_cst)
#define atomic_compare_exchange_weak(ptr, exch, val) __c11_atomic_compare_exchange_weak(ptr, exch, val, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_strong(ptr, exch, val) __c11_atomic_compare_exchange_strong(ptr, exch, val, memory_order_seq_cst, memory_order_seq_cst)
#else

#define atomic_fetch_add(p,x) _Generic((p), \
                                _Atomic(uint64_t) *: atomicAddU64, \
                                _Atomic(uint32_t) *: atomicAddU32, \
                                _Atomic(uint16_t) *: atomicAddU16, \
                                _Atomic(uint8_t) *: atomicAddU8,   \
                                int *: atomicAddInt)(p,x)

int atomicAddInt(int *p, int x);
uint64_t atomicAddU64(_Atomic(uint64_t) *p, uint64_t x);
uint32_t atomicAddU32(_Atomic(uint32_t) *p, uint32_t x);
uint16_t atomicAddU16(_Atomic(uint16_t) *p, uint16_t x);
uint8_t atomicAddU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_fetch_sub(p,x) _Generic((p), \
                                _Atomic(uint64_t) *: atomicSubU64, \
                                _Atomic(uint32_t) *: atomicSubU32, \
                                _Atomic(uint16_t) *: atomicSubU16, \
                                _Atomic(uint8_t) *: atomicSubU8,   \
                                int *: atomicSubInt)(p,x)

int atomicSubInt(int *p, int x);
uint64_t atomicSubU64(_Atomic(uint64_t) *p, uint64_t x);
uint32_t atomicSubU32(_Atomic(uint32_t) *p, uint32_t x);
uint16_t atomicSubU16(_Atomic(uint16_t) *p, uint16_t x);
uint8_t atomicSubU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_fetch_and(p,x) _Generic((p), \
                                _Atomic(uint64_t) *: atomicAndU64, \
                                _Atomic(uint32_t) *: atomicAndU32, \
                                _Atomic(uint16_t) *: atomicAndU16, \
                                _Atomic(uint8_t) *: atomicAndU8,   \
                                int *: atomicAndInt)(p,x)

int atomicAndInt(int *p, int x);
uint64_t atomicAndU64(_Atomic(uint64_t) *p, uint64_t x);
uint32_t atomicAndU32(_Atomic(uint32_t) *p, uint32_t x);
uint16_t atomicAndU16(_Atomic(uint16_t) *p, uint16_t x);
uint8_t atomicAndU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_fetch_or(p,x) _Generic((p), \
                                _Atomic(uint64_t) *: atomicOrU64, \
                                _Atomic(uint32_t) *: atomicOrU32, \
                                _Atomic(uint16_t) *: atomicOrU16, \
                                _Atomic(uint8_t) *: atomicOrU8,   \
                                int *: atomicOrInt)(p,x)

int atomicOrInt(int *p, int x);
uint64_t atomicOrU64(_Atomic(uint64_t) *p, uint64_t x);
uint32_t atomicOrU32(_Atomic(uint32_t) *p, uint32_t x);
uint16_t atomicOrU16(_Atomic(uint16_t) *p, uint16_t x);
uint8_t atomicOrU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_fetch_xor(p,x) _Generic((p), \
                                _Atomic(uint64_t) *: atomicXorU64, \
                                _Atomic(uint32_t) *: atomicXorU32, \
                                _Atomic(uint16_t) *: atomicXorU16, \
                                _Atomic(uint8_t) *: atomicXorU8,   \
                                int *: atomicXorInt)(p,x)

int atomicXorInt(int *p, int x);
uint64_t atomicXorU64(_Atomic(uint64_t) *p, uint64_t x);
uint32_t atomicXorU32(_Atomic(uint32_t) *p, uint32_t x);
uint16_t atomicXorU16(_Atomic(uint16_t) *p, uint16_t x);
uint8_t atomicXorU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_store(p,x) _Generic((p), \
                                   _Atomic(uint64_t) *: atomicStoreU64, \
                                   _Atomic(uint32_t) *: atomicStoreU32, \
                                   _Atomic(uint16_t) *: atomicStoreU16, \
                                   _Atomic(uint8_t) *: atomicStoreU8,   \
                                   int *: atomicStoreInt)(p,x)

void atomicStoreInt(int *p, int x);
void atomicStoreU64(_Atomic(uint64_t) *p, uint64_t x);
void atomicStoreU32(_Atomic(uint32_t) *p, uint32_t x);
void atomicStoreU16(_Atomic(uint16_t) *p, uint16_t x);
void atomicStoreU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_load(p) _Generic((p), \
                                _Atomic(uint64_t) *: atomicLoadU64, \
                                _Atomic(uint32_t) *: atomicLoadU32, \
                                _Atomic(uint16_t) *: atomicLoadU16, \
                                _Atomic(uint8_t) *: atomicLoadU8,   \
                                int *: atomicLoadInt)(p)

int atomicLoadInt(int *p);
uint64_t atomicLoadU64(_Atomic(uint64_t) *p);
uint32_t atomicLoadU32(_Atomic(uint32_t) *p);
uint16_t atomicLoadU16(_Atomic(uint16_t) *p);
uint8_t atomicLoadU8(_Atomic(uint8_t) *p);

#define atomic_exchange(p,x) _Generic((p), \
                                _Atomic(uint64_t) *: atomicExchangeU64, \
                                _Atomic(uint32_t) *: atomicExchangeU32, \
                                _Atomic(uint16_t) *: atomicExchangeU16, \
                                _Atomic(uint8_t) *: atomicExchangeU8,   \
                                int *: atomicExchangeInt)(p,x)

int atomicExchangeInt(int *p, int x);
uint64_t atomicExchangeU64(_Atomic(uint64_t) *p, uint64_t x);
uint32_t atomicExchangeU32(_Atomic(uint32_t) *p, uint32_t x);
uint16_t atomicExchangeU16(_Atomic(uint16_t) *p, uint16_t x);
uint8_t atomicExchangeU8(_Atomic(uint8_t) *p, uint8_t x);

#define atomic_compare_exchange_strong(p,exp,desired) _Generic((p), \
                                _Atomic(uint64_t) *: atomicCompareExchangeStrongU64, \
                                _Atomic(uint32_t) *: atomicCompareExchangeStrongU32, \
                                _Atomic(uint16_t) *: atomicCompareExchangeStrongU16, \
                                _Atomic(uint8_t) *: atomicCompareExchangeStrongU8,   \
                                int *: atomicCompareExchangeStrongInt)(p,exp,desired)


_Bool atomicCompareExchangeStrongInt(int *p, int *exp, int desired);
_Bool atomicCompareExchangeStrongU64(_Atomic(uint64_t) *p, uint64_t *exp, uint64_t desired);
_Bool atomicCompareExchangeStrongU32(_Atomic(uint32_t) *p, uint32_t *exp, uint32_t desired);
_Bool atomicCompareExchangeStrongU16(_Atomic(uint16_t) *p, uint16_t *exp, uint16_t desired);
_Bool atomicCompareExchangeStrongU8(_Atomic(uint8_t) *p, uint8_t *exp, uint8_t desired);


#endif


typedef struct fake_uint128_tag{
  uint64_t hi;
  uint64_t lo;
} fake_uint128_t;

typedef struct fake_int128_tag{
  int64_t hi;
  int64_t lo;
} fake_int128_t;

#define joe_mul128(x,y) 0
#define joe_castU128To64(x) (x)->lo

#else

#define joe_mul128(x,y) ((x) * (y))
#define joe_castU128To64(x) (x)

#endif 

#if __CHARSET_LIB == 1
#define QASCII 1
#endif

#include <time.h>
#include <stdbool.h>

void printf2(char *fmt, ...);

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

void qdumpbuffer(void *buffer, int length); /* temp hack */

int convertOpenStream(int fd, unsigned short fileCCSID);

#define CHARSET_ISO8859 819

int tagFile(const char *pathname, unsigned short ccsid);

#define EXTATTR_MASK 0x1F
#define EXTATTR_SHARELIB 0x10
#define EXTATTR_NO_SHAREAS 0x08
#define EXTATTR_APF_AUTH 0x04
#define EXTATTR_PROGCTL 0x02
#define EXTATTR_EXTERNAL_SYMLINK 0x01

int changeExtendedAttributes(const char *pathname, int attribute, bool onOff);

#endif

