/*
 * wod_thread.c
 *
 *  Created on: 2013-3-4
 *      Author: wd
 */

#include <pthread.h>
#include "wod_thread.h"
#include "wod_errno.h"
struct wod_thread
{
	pthread_t pid;
	void *arg;
	wod_thread_proc func;
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
	pthread_create(&(*thread)->pid,&attr,wod_thread_proc,*thread);
	return WOD_OK;
}
void
wod_thread_join(wod_thread_t *thread,void **state)
{
	pthread_join(thread->pid,state);
}
void
wod_thread_detach(wod_thread_t *thread,int *state)
{
	pthread_detach(thread->pid);
}
void
wod_thread_exit()
{

}
void
wod_thread_yeild()
{

}


