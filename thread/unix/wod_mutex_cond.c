/*
 * wod_mutex_cond.c
 *
 *  Created on: 2013-3-4
 *      Author: wd
 */
#include <pthread.h>
#include <stdlib.h>
#include "wod_thread.h"
#include "wod_errno.h"

struct wod_mutex{
	pthread_mutex_t mtx;
};
wod_ret_t
wod_mutex_create(wod_mutex_t **mtx)
{
	wod_mutex_t * p = malloc(sizeof(wod_mutex_t));
	pthread_mutex_init(&p->mtx,NULL);
	*mtx = p;
	return WOD_OK;
}
wod_ret_t
wod_mutex_lock(wod_mutex_t *mtx)
{
	int err =pthread_mutex_lock(&mtx->mtx);
	if(  err == 0){
		return WOD_OK;
	}
	return errno;
}
wod_ret_t
wod_mutex_trylock(wod_mutex_t *mtx)
{
	int err =pthread_mutex_trylock(&mtx->mtx);
	if(  err == 0){
		return WOD_OK;
	}
	return err;
}
wod_ret_t
wod_mutex_unlock(wod_mutex_t *mtx)
{
	int err =pthread_mutex_unlock(&mtx->mtx);
	if(  err == 0){
		return WOD_OK;
	}
	return err;
}
wod_ret_t
wod_mutext_lock_timeout(wod_mutex_t*mtx,long long timeout)
{
	struct timespec tm;
	tm.tv_sec = timeout/1000000;
	tm.tv_nsec = timeout-tm.tv_sec;
	int err = pthread_mutex_timedlock(&mtx->mtx,&tm);
	if(  err == 0){
		return WOD_OK;
	}
	return err;
}
void
wod_mutex_destroy(wod_mutex_t *mtx)
{
	pthread_mutex_destroy(&mtx->mtx);
	free(mtx);
}

struct wod_cond
{
	pthread_cond_t cond;
};
wod_ret_t
wod_cond_create(wod_cond_t **cond)
{
	wod_cond_t * c = malloc(sizeof(wod_cond_t));
	pthread_cond_init(&c->cond,NULL);
	*cond = c;
	return WOD_OK;
}
wod_ret_t
wod_cond_signal(wod_cond_t *cond)
{
	int er = pthread_cond_signal(&cond->cond);
	if(er == 0)
		return WOD_OK;
	return er;
}
wod_ret_t
wod_cond_wait(wod_cond_t*cond,wod_mutex_t *mtx)
{
	int er = pthread_cond_wait(&cond->cond,&mtx->mtx);
	if(er == 0)
		return WOD_OK;
	return er;
}
wod_ret_t
wod_cond_wait_timeout(wod_cond_t*cond,wod_mutex_t *mtx,wod_i64_t timeout)
{
	struct timespec tm;
	tm.tv_sec = timeout/1000000;
	tm.tv_nsec = timeout-tm.tv_sec;
	int er = pthread_cond_timedwait(&cond->cond,&mtx->mtx,&tm);
	if(er == 0)
		return WOD_OK;
	return er;
}
wod_ret_t
wod_cond_broadcast(wod_cond_t *cond)
{
	int er = pthread_cond_broadcast(&cond->cond);
	if(er == 0)
		return WOD_OK;
	return er;
}
void
wod_cond_destroy(wod_cond_t *cond)
{
	pthread_cond_destroy(&cond->cond);
	free(cond);
}
