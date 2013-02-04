/*
 * wv.c
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#include "evinner.h"
#include "ev.h"
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static int _initPollor(struct wvPoller* pllor,int type);
static inline void _wv_sleep(int usec){
	if( usec <= 0 ){
		return ;
	}
	struct timeval tv;
	tv.tv_sec = usec/1000000;
	tv.tv_usec = usec - tv.tv_sec*1000000;
	select(0,NULL,NULL,NULL,&tv);
}
static double inline wvGetTime(){
	struct timeval time;
	gettimeofday(&time,NULL);
	return time.tv_sec + time.tv_usec * 1E-6;
}
static inline int _hashFunction(int id){
	return id%HASH_SIZE;
}
struct wvLoop * wvLoopNew(int set_size,int type){
	struct wvLoop * loop = malloc(sizeof(struct wvLoop));
	int ret = _initPollor(&loop->pollor,type);
	if(ret != WV_ROK){
		free(loop);
		return NULL;
	}
	ret = loop->pollor.New(loop,0);
	if(ret != WV_ROK){
		free(loop);
		return NULL;
	}
	loop->set_size = set_size;
	loop->idIndex = loop->set_size;
	loop->userdefHead = NULL;
	loop->files = malloc(sizeof(struct wvIO) *set_size);
	loop->pendFds = malloc(sizeof(int) *set_size);
	int i=0;
	for(i=0; i< loop->set_size; i++){
		loop->files[i].event = WV_NONE;
	}
	memset(loop->hashMap,0,sizeof(loop->hashMap));
	loop->isQuit = 0;
	loop->used = 0;
	loop->minSec = SLEEP;
	return loop;
}
void wvLoopDelete(struct wvLoop *loop){
	loop->pollor.Del(loop);
	free(loop);
}
static void _processIO(struct wvLoop *loop,double runSec){
	double tmpSec = loop->minSec - runSec;
	if(tmpSec < 0.0 ){
		tmpSec = 0.0;
	}
	if(loop->used){
		int ret = loop->pollor.Poll(loop,tmpSec);
		struct wvIO * fev;
		for(;ret>0;ret--){
			fev = &loop->files[loop->pendFds[ret-1]];
			if(fev->event & fev->revent & WV_IO_READ){
				fev->readProc(fev->readArg,WV_IO_READ);
			}
			if(fev->event & fev->revent & WV_IO_WRITE){
				fev->writeProc(fev->writeArg,WV_IO_WRITE);
			}
		}
	}else{
		int usec = tmpSec*1E6 - 100;
		_wv_sleep(usec);
	}
}
static void _processIdle(struct wvLoop *loop){
	struct wvUserDef*tmp=loop->userdefHead,*pre = NULL,*next;
	while(tmp){
		next = tmp->next;
		if(!tmp->dispose){
			tmp->idluceProc(tmp->userdefArg);
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
static void _processTime(struct wvLoop *loop){
	int i =0;
	loop->minSec = SLEEP;
	for(i=0;i<HASH_SIZE;i++){
		struct wvTime*tmp=loop->hashMap[i],*pre = NULL,*next;
		while(tmp){
			next = tmp->next;
			if(!tmp->dispose){
				double cutClock = wvGetTime();
				tmp->passSec += cutClock-tmp->cutClock;
				tmp->cutClock = cutClock;
				double tmpSec;
				if(tmp->passSec >= tmp->sec){
					tmp->passSec = 0.0;
					tmp->dispose = tmp->timeProc(tmp->timeArg);
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
void wvRun(struct wvLoop *loop){
	double space = 0.0;
	while(!loop->isQuit){
		double  preSec = wvGetTime();
		_processTime(loop);
		_processIO(loop,space);
		_processIdle(loop);
		space =  wvGetTime() - preSec;
	}
}
void wvStop(struct wvLoop *loop){
	loop->isQuit = 1;
}

int wvIOAdd(struct wvLoop *loop,int fd,int event,wvIOFn cb,void *cbArg){
	if(fd > loop->set_size || fd <= 0 || !cb){
		return -EINVAL;
	}
	struct wvIO *pFile = &loop->files[fd];
	pFile->fd = fd;
	int ret = loop->pollor.Add(loop,fd,event);
	if(ret != WV_ROK){
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
void wvIORemove(struct wvLoop *loop,int id,int event){
	if(id > loop->set_size){
		return ;
	}
	struct wvIO *pFile = &loop->files[id];
	if(pFile->event & event){
		loop->pollor.Remove(loop,pFile->fd,event);
		pFile->event &= (~event);
		if(pFile->event == WV_NONE){
			loop->used --;
		}
	}
}

int wvTimeAdd(struct wvLoop *loop,int sec,int usec,wvTimeFn cb,void *cbArg){
	if(!cb){
		return -EINVAL;
	}
	struct wvTime * pTime = malloc(sizeof(struct wvTime));
	pTime->id = loop->idIndex++;
	pTime->cutClock = wvGetTime();
	pTime->sec = sec + usec*1E-6;
	pTime->passSec = 0.0;
	pTime->timeArg = cbArg;
	pTime->timeProc = cb;
	pTime->dispose = 0;
	int hash = _hashFunction(pTime->id);
	pTime->next = loop->hashMap[hash];
	loop->hashMap[hash] = pTime;
	return pTime->id;
}
void wvTimeRemove(struct wvLoop * loop,int id){
	int hash = _hashFunction(id);
	struct wvTime*tmp=loop->hashMap[hash];
	while(tmp){
		if(tmp->id == id){
			tmp->dispose = 1;
		}
		tmp = tmp->next;
	}
}

int wvUserDefAdd(struct wvLoop *loop,wvUserDefFn cb,void *cbArg){
	if(!cb){
		return -EINVAL;
	}
	struct wvUserDef * pIdle = malloc(sizeof(struct wvUserDef));
	pIdle->next = loop->userdefHead;
	loop->userdefHead  = pIdle;
	pIdle->id = loop->idIndex++;
	pIdle->userdefArg = cbArg;
	pIdle->idluceProc = cb;
	pIdle->dispose = 0;
	return pIdle->id;
}
void wvUserDefRemove(struct wvLoop *loop,int id){
	struct wvUserDef*tmp=loop->userdefHead;
	while(tmp){
		if(tmp->id == id){
			tmp->dispose = 1;
		}
		tmp = tmp->next;
	}
}

#define HAS_SELECT 1
#if HAS_EPOLL
#include "ev_epoll.c"
#endif
#if HAS_SELECT
#include "ev_select.c"
#endif
static int _initPollor(struct wvPoller* pllor,int type){
	if(type == WV_POLL_EPOLL){
#if HAS_EPOLL
		SET_POLLER(poller,epoll);
		return WV_ROK;
#endif
		return -EINVAL;
	}else if(type == WV_POLL_SELECT){
#if HAS_SELECT
		SET_POLLER(poller,select);
		return WV_ROK;
#endif
		return -EINVAL;
	}
	return -EINVAL;
}
