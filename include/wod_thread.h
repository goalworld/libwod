#ifndef _WOD_THREAD_H_
#define _WOD_THREAD_H_
#include <stddef.h>
#include "wod_type.h"
typedef struct wod_thread wod_thread_t;
typedef void* (*wod_thread_proc)(void *argc);
wod_ret_t wod_thread_create(wod_thread_t ** thread,wod_thread_proc,void*arg,size_t stack_size);
wod_ret_t wod_thread_join(wod_thread_t *thread,void **state);
wod_ret_t wod_thread_detach(wod_thread_t *thread);
void wod_thread_exit(wod_thread_t *thread,void *retvalue);
wod_ret_t wod_thread_yeild();

typedef struct wod_mutex wod_mutex_t;
wod_ret_t wod_mutex_create(wod_mutex_t **mtx);
wod_ret_t wod_mutex_lock(wod_mutex_t *mtx);
wod_ret_t wod_mutex_trylock(wod_mutex_t *mtx);
wod_ret_t wod_mutex_unlock(wod_mutex_t *mtx);
wod_ret_t wod_mutext_lock_timeout(wod_mutex_t*mtx,long long timeout);
void wod_mutex_destroy(wod_mutex_t *mtx);

typedef struct wod_cond wod_cond_t;
wod_ret_t wod_cond_create(wod_cond_t **cond);
wod_ret_t wod_cond_signal(wod_cond_t *cond);
wod_ret_t wod_cond_wait(wod_cond_t*cond,wod_mutex_t *mtx);
wod_ret_t wod_cond_wait_timeout(wod_cond_t*cond,wod_mutex_t *mtx,wod_i64_t timeout);
wod_ret_t wod_cond_broadcast(wod_cond_t *cond);
void wod_cond_destroy(wod_cond_t *cond);

typedef struct wod_rwlock wod_rwlock_t;
wod_ret_t wod_rwlock_create(wod_rwlock_t **rwlock);
wod_ret_t wod_rwlock_rlock(wod_rwlock_t *rwlock);
wod_ret_t wod_rwlock_rtrylock(wod_rwlock_t *rwlock);
wod_ret_t wod_rwlock_unlock(wod_rwlock_t *rwlock);
wod_ret_t wod_rwlock_rlock_timeout(wod_rwlock_t *rwlock,long long timeout);
wod_ret_t wod_rwlock_wlock(wod_rwlock_t *rwlock);
wod_ret_t wod_rwlock_wtrylock(wod_rwlock_t *rwlock);
wod_ret_t wod_rwlock_wlock_timeout(wod_rwlock_t *rwlock,long long timeout);
void wod_rwlock_destroy(wod_rwlock_t *cond);


#endif 
