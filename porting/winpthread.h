

/*
  This program and the accompanying materials are
  made available under the terms of the Eclipse Public License v2.0 which accompanies
  this distribution, and is available at https://www.eclipse.org/legal/epl-v20.html
  
  SPDX-License-Identifier: EPL-2.0
  
  Copyright Contributors to the Zowe Project.
*/

/* 
   What BSD thinks

   https://github.com/openbsd/src/blob/master/include/pthread.h
 */

#ifndef __WINPTHREAD__ 
#define __WINPTHREAD__  1

#include <windows.h>
#include <time.h>
/* #include <winsock2.h> */

typedef HANDLE Mutex;
typedef HANDLE Thread;

/*
#define mutexLock(x) WaitForSingleObject(x,INFINITE)
#define mutexUnlock(x) ReleaseMutex(x)
#define mutexCreate(x) (x) = CreateMutexEx(NULL,NULL,0x00000000,MUTEX_ALL_ACCESS);
*/

struct pthread;
struct pthread_attr;
struct pthread_cond_attr;
struct pthread_mutex_attr;
struct pthread_once;
struct pthread_rwlock;
struct pthread_rwlockattr;

typedef struct pthread_attr_tag {
  int foo;
} pthread_attr_t;

typedef struct pthread_cond {
  CONDITION_VARIABLE condVar;
} pthread_cond_t;

typedef struct pthread_tag {
  HANDLE windowsThread;
  int    threadID;
} pthread_t;

typedef struct pthread_mutex {
  /* HANDLE windowsMutex;  Mutex's can be used inter-process, critical_section within a process */
  CRITICAL_SECTION criticalSection;
} pthread_mutex_t;

typedef struct	pthread_mutex_attr   *pthread_mutexattr_t;
typedef struct	pthread_cond_attr	*pthread_condattr_t;

#define PTHREAD_MUTEX_INITIALIZER	{ 0, 0 }
#define PTHREAD_COND_INITIALIZER	NULL
#define PTHREAD_RWLOCK_INITIALIZER	NULL

int pthread_mutex_destroy(pthread_mutex_t *);
int pthread_mutex_init(pthread_mutex_t *,
                       const pthread_mutexattr_t *);
int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_timedlock(pthread_mutex_t *,
                            const struct timespec *);
int pthread_mutex_trylock(pthread_mutex_t *);
int pthread_mutex_unlock(pthread_mutex_t *);

int pthread_mutexattr_init(pthread_mutexattr_t *);
int pthread_mutexattr_destroy(pthread_mutexattr_t *);
int pthread_mutexattr_gettype(pthread_mutexattr_t *, int *);
int pthread_mutexattr_settype(pthread_mutexattr_t *, int);

/* Threads */

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine) (void *), void *arg);

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_cond_broadcast(pthread_cond_t *);
int pthread_cond_destroy(pthread_cond_t *);
int pthread_cond_init(pthread_cond_t *,
                      const pthread_condattr_t *);
int pthread_cond_signal(pthread_cond_t *);
int pthread_cond_timedwait(pthread_cond_t *,
                           pthread_mutex_t *, const struct timespec *);
int pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);

#endif 
