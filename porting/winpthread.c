#include <stdio.h>
#include "winpthread.h"
#include <windows.h>
#include <math.h>

/* 
#define mutexLock(x) WaitForSingleObject(x,INFINITE)
#define mutexUnlock(x) ReleaseMutex(x)
#define mutexCreate(x) (x) = CreateMutexEx(NULL,NULL,0x00000000,MUTEX_ALL_ACCESS);
*/

int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr){
  int initStatus = InitializeCriticalSectionAndSpinCount(&mutex->criticalSection, 0x00000400);
  if (!initStatus){ /* Win2003 and XP only */
    return 24;
  } else {
    return 0;
  }
}

int pthread_mutex_lock(pthread_mutex_t *mutex){
  /*  mutex->windowsMutex = CreateMutexEx(NULL,NULL,0x00000000,MUTEX_ALL_ACCESS); */
  /* WaitForSingleObject(mutex->windowsMutex,INFINITE); */
  EnterCriticalSection(&mutex->criticalSection);
  return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex){
  /* ReleaseMutex(mutex->windowsMutex); */
  LeaveCriticalSection(&mutex->criticalSection);
  return 0;
}

/* reference */

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine) (void *),
                   void *arg){
  printf("write pthread_create(), please\n");
  return 0;
}


int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr){
    (void)attr;
    if (cond == NULL){
      return 12;
    }
    InitializeConditionVariable(&cond->condVar);
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond){
  (void)cond;
  return 0;  /* n/a in windows */
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){
  if (cond == NULL || mutex == NULL){
    return 12;
  }
  return pthread_cond_timedwait(cond, mutex, NULL);
}

int pthread_cond_timedwait(pthread_cond_t *cond,
                           pthread_mutex_t *mutex,
                           const struct timespec *abstime){
  if (cond == NULL || mutex == NULL){
    return 12;
  }
  long ms = 0;
  if (abstime){
    ms = round(abstime->tv_nsec/1.0e6);
  }
  if (!SleepConditionVariableCS(&cond->condVar, &mutex->criticalSection, ms)){
    return 16;
  }
  return 0;
}
  
int pthread_cond_signal(pthread_cond_t *cond){
  if (cond == NULL){
    return 12;
  }
  WakeConditionVariable(&cond->condVar);
  return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond){
  if (cond == NULL){
    return 12;
  }
  WakeAllConditionVariable(&cond->condVar);
  return 0;
}


/* 
quickjs-835b20.o : error LNK2019: unresolved external symbol pthread_cond_init referenced in function js_atomics_wait
quickjs-835b20.o : error LNK2019: unresolved external symbol pthread_cond_wait referenced in function js_atomics_wait
quickjs-835b20.o : error LNK2019: unresolved external symbol pthread_cond_timedwait referenced in function js_atomics_wait
quickjs-835b20.o : error LNK2019: unresolved external symbol pthread_cond_destroy referenced in function js_atomics_wait
quickjs-835b20.o : error LNK2019: unresolved external symbol pthread_cond_signal referenced in function js_atomics_notify

*/
