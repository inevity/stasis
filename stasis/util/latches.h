#include <stasis/common.h>
#include <stasis/util/latchStats.h>

#ifndef __LATCHES_H
#define __LATCHES_H

/**
   A data structure for profiling latching behavior.
   All time values recorded in this struct are in microseconds.
 */
typedef struct {
  const char * file;
  int line;
  const char * name;
  pthread_mutex_t mutex;
  profile_tuple tup;
  char * last_acquired_at;
  /*  pblHashTable_t * lockpoints; */
  void * lockpoints;
} lladd_pthread_mutex_t;

#include "rw.h"

/**
   Keeps some profiling information along with a read/write lock.
*/

typedef struct {
  const char * file;
  int line;
  rwl * lock;
  profile_tuple tup;
  char * last_acquired_at;
  /* pblHashTable_t * lockpoints; */
  void * lockpoints;
  pthread_t holder;
  int readCount;
} __profile_rwl;

#ifdef PROFILE_LATCHES

/*#define pthread_mutex_t lladd_pthread_mutex_t

#define pthread_mutex_init(x, y) __lladd_pthread_mutex_init((x), (y), __FILE__, __LINE__, #x)
#define pthread_mutex_destroy(x) __lladd_pthread_mutex_destroy((x))
#define pthread_mutex_lock(x) __lladd_pthread_mutex_lock((x), __FILE__, __LINE__)
#define pthread_mutex_unlock(x) __lladd_pthread_mutex_unlock((x))
#define pthread_mutex_trylock(x) NO_PROFILING_EQUIVALENT_TO_PTHREAD_TRYLOCK
#define pthread_cond_wait(x, y) __lladd_pthread_cond_wait((x), (y), __FILE__, __LINE__, #x, #y)
#define pthread_cond_timedwait(x, y, z) __lladd_pthread_cond_timedwait((x), (y), (z), __FILE__, __LINE__, #x, #y)

int __lladd_pthread_mutex_init(lladd_pthread_mutex_t  *mutex,  const  pthread_mutexattr_t *mutexattr, const char * file, int line, const char * mutex_name);
int __lladd_pthread_mutex_lock(lladd_pthread_mutex_t *mutex, char * file, int line);
int __lladd_pthread_mutex_unlock(lladd_pthread_mutex_t *mutex);
int __lladd_pthread_mutex_destroy(lladd_pthread_mutex_t *mutex);
int __lladd_pthread_cond_wait(pthread_cond_t *cond, lladd_pthread_mutex_t *mutex, 
			      char * file, int line, char * cond_name, char * mutex_name);
// @param abstime should be const struct timespec, but GCC won't take that. 
int __lladd_pthread_cond_timedwait(pthread_cond_t *cond, lladd_pthread_mutex_t *mutex, void *abstime,
				   char * file, int line, char * cond_name, char * mutex_name);
*/
#define initlock() __profile_rw_initlock(__FILE__, __LINE__)
#define readlock(x, y) __profile_readlock((x),(y), __FILE__, __LINE__)
#define writelock(x, y) __profile_writelock((x), (y), __FILE__, __LINE__)
#define readunlock(x) __profile_readunlock((x))
#define writeunlock(x) __profile_writeunlock((x))
#define unlock(x) __profile_unlock((x))
#define downgradelock(x) __profile_downgradelock((x))
#define deletelock(x) __profile_deletelock((x))

#define rwl __profile_rwl

rwl *__profile_rw_initlock (char * file, int line);
void __profile_readlock (rwl *lock, int d, char * file, int line);
void __profile_writelock (rwl *lock, int d, char * file, int line);
void __profile_readunlock (rwl *lock);
void __profile_writeunlock (rwl *lock);
void __profile_unlock (rwl *lock);
void __profile_downgradelock (rwl *lock);
void __profile_deletelock (rwl *lock);




#endif  

#ifdef NO_LATCHES



/* #define pthread_mutex_init(x, y) */
/* #define pthread_mutex_destroy(x) */
#define pthread_mutex_lock(x)  1
#define pthread_mutex_unlock(x) 1
#define pthread_mutex_trylock(x) 1
#define pthread_cond_wait(x, y)  1
#define pthread_cond_timedwait(x, y, z)  1

/* #define initlock() */
#define readlock(x, y) 1
#define writelock(x, y) 1
#define readunlock(x) 1
#define writeunlock(x) 1
#define unlock(x)  1
#define downgradelock(x) 1
/* #define deletelock(x) */



#endif

#ifdef HAVE_GCC_ATOMICS
#define CAS(mutex,_a,_o,_n) __sync_bool_compare_and_swap(_a,_o,_n)
#define BARRIER() __sync_synchronize()
#define FETCH_AND_ADD(_o,_i) __sync_fetch_and_add(_o,_i)
#if ULONG_MAX <= 4294967295 // are we on a 32 bit machine?
#define ATOMIC_READ_64(mutex,_o)     FETCH_AND_ADD(_o,0)
#define ATOMIC_WRITE_64(mutex,_o,_n) __sync_lock_test_and_set(_o,_n)
#else // this is a 33 or greater bit machine.  Assume it's 64 bit, and that 64 bit writes are atomic.
#define ATOMIC_READ_64(mutex,_a)    *_a
#define ATOMIC_WRITE_64(mutex,_a,_n) do {*_a=_n; } while (0)
#endif
#define ATOMIC_READ_32(mutex,_a)    *_a
#define ATOMIC_WRITE_32(mutex,_a,_n) do {*_a=_n; } while (0)
#else
#define CAS(mutex,_a,_o,_n)       GCC_ATOMICS_REQUIRED
#define BARRIER()                 GCC_ATOMICS_REQUIRED
#define FETCH_AND_ADD(_a,_i)      GCC_ATOMICS_REQUIRED
#define ATOMIC_READ_64(mutex, _a) GCC_ATOMICS_REQUIRED
#define ATOMIC_WRITE_64(mutex,_a,_n) GCC_ATOMICS_REQUIRED
#endif


#endif /* __LATCHES_H */
