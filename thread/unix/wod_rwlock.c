/*
 * wod_rwlock.c
 *
 *  Created on: 2013-3-4
 *      Author: wd
 */
#include <pthread.h>
#include <stdlib.h>
#include "wod_thread.h"
#include "wod_errno.h"
struct wod_rwlock
{
	pthread_rwlock_t rw;
};
wod_ret_t
wod_rwlock_create(wod_rwlock_t **rwlock)
{
	wod_rwlock_t * p = malloc(sizeof(wod_rwlock_t));
	if(p == NULL){
		return WOD_ENOMEM;
	}
	int er = pthread_rwlock_init(&p->rw,NULL);
	if(er != 0) return er;
	return WOD_OK;
}
wod_ret_t
wod_rwlock_rlock(wod_rwlock_t *rwlock)
{
	return pthread_rwlock_rdlock(&rwlock->rw);
}
wod_ret_t
wod_rwlock_rtrylock(wod_rwlock_t *rwlock)
{
	return pthread_rwlock_tryrdlock(&rwlock->rw);
}
wod_ret_t
wod_rwlock_unlock(wod_rwlock_t *rwlock)
{
	return pthread_rwlock_unlock(&rwlock->rw);
}
wod_ret_t
wod_rwlock_rlock_timeout(wod_rwlock_t *rwlock,long long timeout)
{
	struct timespec tm;
	tm.tv_sec = timeout/1000000;
	tm.tv_nsec = timeout-tm.tv_sec;
	return pthread_rwlock_timedrdlock(&rwlock->rw,&tm);
}
wod_ret_t
wod_rwlock_wlock(wod_rwlock_t *rwlock)
{
	return pthread_rwlock_wrlock(&rwlock->rw);
}
wod_ret_t
wod_rwlock_wtrylock(wod_rwlock_t *rwlock)
{
	return pthread_rwlock_trywrlock(&rwlock->rw);
}
wod_ret_t
wod_rwlock_wlock_timeout(wod_rwlock_t *rwlock,long long timeout)
{
	struct timespec tm;
	tm.tv_sec = timeout/1000000;
	tm.tv_nsec = timeout-tm.tv_sec;
	return pthread_rwlock_timedwrlock(&rwlock->rw,&tm);
}
void
wod_rwlock_destroy(wod_rwlock_t *rwlock)
{
	pthread_rwlock_destroy(&rwlock->rw);
	free(rwlock);
}
