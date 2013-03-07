/*
 * ev.c
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "wod_config.h"
#include "wod_evinner.h"
#include "wod_time.h"
#include "wod_errno.h"
static int _init_pollor(struct wod_event_pollor* pllor,int type);


static inline int
_hash_function(int id)
{
	return id%HASH_SIZE;
}
wod_ret_t
wod_event_create(wod_event_t **ev,int set_size,int type)
{
	struct wod_event * loop = malloc(sizeof(struct wod_event));
	if(loop == NULL){
		return WOD_ENOMEM;
	}
	wod_ret_t ret = _init_pollor(&loop->pollor,type);
	if(ret != WOD_OK){
		free(loop);
		return ret;
	}
	loop->set_size = set_size;
	loop->idIndex = loop->set_size;
	loop->userdefHead = NULL;
	ret = loop->pollor.new(loop,0);
	if(ret != WOD_OK){
		free(loop);
		return ret;
	}


	loop->files = malloc(sizeof(struct wod_event_io) *set_size);
	loop->pendFds = malloc(sizeof(int) *set_size);
	int i=0;
	for(i=0; i< loop->set_size; i++){
		loop->files[i].event = WV_NONE;
	}
	memset(loop->hashMap,0,sizeof(loop->hashMap));
	loop->isQuit = 0;
	loop->used = 0;
	loop->minSec = SLEEP;
	*ev = loop;
	return WOD_OK;
}
void
wodEvLoopDelete(wod_event_t *loop)
{
	loop->pollor.delete(loop);
	free(loop);
}
static void
_processIO(wod_event_t *loop)
{
	long long tmpsec = loop->minSec;
	if(loop->minSec > 1000){
		tmpsec -= 1000;
	}
	if(loop->used){
		int ret = loop->pollor.poll(loop,tmpsec);
		struct wod_event_io * fev;
		for(;ret>0;ret--){
			fev = &loop->files[loop->pendFds[ret-1]];
			if(fev->event & fev->revent & WV_IO_READ){
				fev->readProc(loop,fev->readArg,WV_IO_READ);
			}
			if(fev->event & fev->revent & WV_IO_WRITE){
				fev->writeProc(loop,fev->writeArg,WV_IO_WRITE);
			}
		}
	}else{
		wod_usleep(tmpsec);
	}
}
static void _processIdle(wod_event_t *loop){
	struct wod_event_userdef*tmp=loop->userdefHead,*pre = NULL,*next;
	while(tmp){
		next = tmp->next;
		if(!tmp->dispose){
			tmp->idluceProc(loop,tmp->userdefArg);
		}else{
			if(pre){
				pre->next = next;
			}else{
				loop->userdefHead = next;
			}
			free(tmp);
		}
		pre = tmp;
		tmp = next;
	}
}
static void _processTime(wod_event_t *loop){
	int i =0;
	loop->minSec = SLEEP;
	for(i=0;i<HASH_SIZE;i++){
		struct wod_event_time*tmp=loop->hashMap[i],*pre = NULL,*next;
		while(tmp){
			next = tmp->next;
			if(!tmp->dispose){
				long long  cutClock = wod_time_usecond();
				tmp->passSec += cutClock-tmp->cutClock;
				tmp->cutClock = cutClock;
				long long  tmpSec;
				if(tmp->passSec >= tmp->sec){
					tmp->passSec = 0.0;
					tmp->dispose = tmp->timeProc(loop,tmp->timeArg);
					tmpSec = tmp->sec;
				}else{
					tmpSec = (tmp->sec - tmp->passSec);
				}
				if(tmpSec < loop->minSec){
					loop->minSec = tmpSec;
				}
			}
			if(tmp->dispose){
				if(pre){
					pre->next = next;
				}else{
					loop->hashMap[i] = next;
				}
				free(tmp);
			}
			pre = tmp;
			tmp = next;
		}
	}
}
void wod_event_once(wod_event_t *loop){
	_processTime(loop);
	_processIdle(loop);
	_processIO(loop);
}
void wod_event_loop(wod_event_t *loop){
	while(!loop->isQuit){
		_processTime(loop);
		_processIdle(loop);
		_processIO(loop);
	}
}
void wod_event_stop(wod_event_t *loop){
	loop->isQuit = 1;
}

int wod_event_io_add(wod_event_t *loop,int fd,int event,wod_event_io_fn cb,void *cbArg){
	if(fd > loop->set_size || fd <= 0 || !cb){
		return -EINVAL;
	}
	struct wod_event_io *pFile = &loop->files[fd];
	pFile->fd = fd;
	int ret = loop->pollor.add(loop,fd,event);
	if(ret != WOD_OK){
		return ret;
	}
	if(pFile->event == WV_NONE){
		loop->used ++ ;
	}
	pFile->event |= event;
	if(event & WV_IO_READ){
		pFile->readArg = cbArg;
		pFile->readProc = cb;
	}
	if(event & WV_IO_WRITE){
		pFile->writeArg = cbArg;
		pFile->writeProc = cb;
	}

	return fd;
}
void wod_event_io_remove(wod_event_t *loop,int id,int event){
	if(id > loop->set_size){
		return ;
	}
	struct wod_event_io *pFile = &loop->files[id];
	if(pFile->event & event){
		loop->pollor.remove(loop,pFile->fd,event);
		pFile->event &= (~event);
		if(pFile->event == WV_NONE){
			loop->used --;
		}
	}
}

int wod_event_time_add(wod_event_t *loop,long long usec,wod_event_time_fn cb,void *cbArg){
	if(!cb){
		return -EINVAL;
	}
	struct wod_event_time * pTime = malloc(sizeof(struct wod_event_time));
	pTime->id = loop->idIndex++;
	pTime->cutClock = wod_time_usecond();
	pTime->sec = usec;
	pTime->passSec = 0.0;
	pTime->timeArg = cbArg;
	pTime->timeProc = cb;
	pTime->dispose = 0;
	int hash = _hash_function(pTime->id);
	pTime->next = loop->hashMap[hash];
	loop->hashMap[hash] = pTime;
	return pTime->id;
}
void wod_event_time_remove(wod_event_t * loop,int id){
	int hash = _hash_function(id);
	struct wod_event_time*tmp=loop->hashMap[hash];
	while(tmp){
		if(tmp->id == id){
			tmp->dispose = 1;
		}
		tmp = tmp->next;
	}
}

int wod_event_userdef_add(wod_event_t *loop,wod_event_userdef_fn cb,void *cbArg){
	if(!cb){
		return -EINVAL;
	}
	struct wod_event_userdef * pIdle = malloc(sizeof(struct wod_event_userdef));
	pIdle->next = loop->userdefHead;
	loop->userdefHead  = pIdle;
	pIdle->id = loop->idIndex++;
	pIdle->userdefArg = cbArg;
	pIdle->idluceProc = cb;
	pIdle->dispose = 0;
	return pIdle->id;
}
void wod_event_userdef_remove(wod_event_t *loop,int id){
	struct wod_event_userdef*tmp=loop->userdefHead;
	while(tmp){
		if(tmp->id == id){
			tmp->dispose = 1;
		}
		tmp = tmp->next;
	}
}

#if HAS_EPOLL
#include "ev_epoll.c"
#endif
#if HAS_SELECT
#include "ev_select.c"
#endif
#if HAS_POLL
#include "ev_poll.c"
#endif
static int _init_pollor(struct wod_event_pollor* pllor,int type){
	if(type == WV_POLL_EPOLL){
#if HAS_EPOLL
		SET_POLLER(poller,epoll);
		return WOD_OK;
#endif
		return -EINVAL;
	}else if(type == WV_POLL_SELECT){
#if HAS_SELECT
		SET_POLLER(poller,select);
		return WOD_OK;
#endif
		return -EINVAL;
	}else if(type == WV_POLL_POLL){
#if HAS_POLL
		SET_POLLER(poller,poll);
		return WOD_OK;
#endif
		return -EINVAL;
	}
	return -EINVAL;
}
