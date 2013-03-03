#ifndef _WOD_THREAD_H_
#define _WOD_THREAD_H_
typedef struct wod_thread wod_thread_t;
typedef int (*wod_thread_proc)(void *argc);
wod_thread_t *wod_thread_new(wod_thread_proc,int stack_size);
void wod_thread_delete(wod_thread_t *thread);
wod_thread_t * wod_thread_self();
void wod_thread_join(wod_thread_t *thread,int *state);
void wod_thread_detach(wod_thread_t *thread,int *state);
void wod_thread_exit();
void wod_thread_yeild();

typedef struct wod_mutex wod_mutex_t;
wod_mutext_t * wod_mutex_new();
int wod_mutex_lock(wod_mutex_t *mtx);
int wod_mutex_trylock(vod_mutex_t *mtx);
int wod_mutex_unlock(wod_mutex_t *mtx);
int wod_mutext_lock_timeout(wod_mutext_t*mtx,long long timeout);
void wod_mutex_delete(wod_mutex_t *mtx);

typedef struct wod_rwlock wod_rwlock_t;
wod_rwlock_t * wod_rwlock_new();
int wod_rwlock_rlock(wod_rwlock_t *rwlock);
int wod_rwlock_rtrylock(wod_rwlock_t *rwlock);
int wod_rwlock_runlock(wod_rwlock_t *rwlock);
int wod_rwlock_rlock_timeout(wod_rwlock_t *rwlock,long long timeout);
int wod_rwlock_wlock(wod_rwlock_t *rwlock);
int wod_rwlock_wtrylock(wod_rwlock_t *rwlock);
int wod_rwlock_wunlock(wod_rwlock_t *rwlock);
int wod_rwlock_wlock_timeout(wod_rwlock_t *rwlock,long long timeout);

typedef struct wod_cond wod_cond_t;
wod_cond_t *wod_cond_new(wod_mutex_t *mtx);
int wod_cond_single(wod_cond_t *cond);
int wod_cond_wait(wod_cond_t*cond);
int wod_cond_broadcast(wod_cond_t *cond);
#endif 
