/*
 * wod_thread.c
 *
 *  Created on: 2013-3-4
 *      Author: wd
 */

#include <pthread.h>
#include <assert.h>
#include <stdlib.h>
#include "wod_thread.h"
#include "wod_errno.h"

struct wod_thread
{
	pthread_t pid;
};
wod_ret_t
wod_thread_create(wod_thread_t ** thread,wod_thread_proc proc,void*arg,size_t stack_size)
{
	*thread= malloc(sizeof(wod_thread_t));
	if(*thread){
		return WOD_ENOMEM;
	}
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,stack_size);
	pthread_create(&(*thread)->pid,&attr,proc,arg);
	return WOD_OK;
}
wod_ret_t
wod_thread_join(wod_thread_t *thread,void **state)
{
	return pthread_join(thread->pid,state);
}
wod_ret_t
wod_thread_detach(wod_thread_t *thread)
{
	return pthread_detach(thread->pid);
}
void
wod_thread_exit(wod_thread_t *thread,void *retvalue)
{
	assert((thread->pid == pthread_self()));
	pthread_exit(&retvalue);
	free(thread);
}
wod_ret_t
wod_thread_yeild()
{
	return sched_yield();
}


