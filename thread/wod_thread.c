/*
 * wod_thread.c
 *
 *  Created on: 2013-3-4
 *      Author: wd
 */

#include <pthread.h>
#include "wod_thread.h"
#include "wod_errno.h"
#include <assert.h>
struct wod_thread
{
	pthread_t pid;
};
int
wod_thread_create(wod_thread_t ** thread,wod_thread_proc,void*arg,size_t stack_size)
{
	*thread= malloc(sizeof(wod_thread_t));
	if(*thread){
		return WOD_ENOMEM;
	}
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,stack_size);
	pthread_create(&(*thread)->pid,&attr,wod_thread_proc,arg);
	return WOD_OK;
}
int
wod_thread_join(wod_thread_t *thread,void **state)
{
	return pthread_join(thread->pid,state);
}
int
wod_thread_detach(wod_thread_t *thread,int *state)
{
	return pthread_detach(thread->pid);
}
void
wod_thread_exit(wod_thread_t *thread,void *retvalue)
{
	assert((thread->pid == pthread_self()));
	pthread_exit(thread->pid,retvalue);
	free(thread);
}
void
wod_thread_yeild()
{
	sched_yield();
}


