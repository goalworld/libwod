/*
 * wodEv.c
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */
#include "wod_ev.h"
#include "evinner.h"
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static int _initPollor(struct wodEvPoller* pllor,int type);
void
wodEvSleep(long long usec)
{
	if( usec <= 0 ){
		return ;
	}
	struct timeval tv;
	tv.tv_sec = usec/1000000;
	tv.tv_usec = usec - tv.tv_sec*1000000;
	select(0,NULL,NULL,NULL,&tv);
}
long long
wodEvGetTime()
{
	struct timeval time;
	gettimeofday(&time,NULL);
	return time.tv_sec*1000000 + time.tv_usec;
}
static inline int
_hashFunction(int id)
{
	return id%HASH_SIZE;
}
struct wodEvLoop *
wodEvLoopNew(int set_size,int type)
{
	struct wodEvLoop * loop = malloc(sizeof(struct wodEvLoop));
	int ret = _initPollor(&loop->pollor,type);
	if(ret != WV_ROK){
		free(loop);
		return NULL;
	}
	loop->set_size = set_size;
	loop->idIndex = loop->set_size;
	loop->userdefHead = NULL;
	ret = loop->pollor.New(loop,0);
	if(ret != WV_ROK){
		free(loop);
		return NULL;
	}


	loop->files = malloc(sizeof(struct wodEvIO) *set_size);
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
void
wodEvLoopDelete(struct wodEvLoop *loop)
{
	loop->pollor.Del(loop);
	free(loop);
}
static void
_processIO(struct wodEvLoop *loop,long long runSec)
{
	long long tmpSec = loop->minSec - runSec;
	if(tmpSec < 0 ){
		tmpSec = 0;
	}
	if(loop->used){
		int ret = loop->pollor.Poll(loop,tmpSec);
		struct wodEvIO * fev;
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
		int usec = tmpSec*1E6 - 100;
		wodEvSleep(usec);
	}
}
static void _processIdle(struct wodEvLoop *loop){
	struct wodEvUserDef*tmp=loop->userdefHead,*pre = NULL,*next;
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
static void _processTime(struct wodEvLoop *loop){
	int i =0;
	loop->minSec = SLEEP;
	for(i=0;i<HASH_SIZE;i++){
		struct wodEvTime*tmp=loop->hashMap[i],*pre = NULL,*next;
		while(tmp){
			next = tmp->next;
			if(!tmp->dispose){
				long long  cutClock = wodEvGetTime();
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
void wodEvOnce(struct wodEvLoop *loop){
	_processTime(loop);
	_processIO(loop,0);
	_processIdle(loop);
}
void wodEvRun(struct wodEvLoop *loop){
	long long  space = 0.0,cut;
	while(!loop->isQuit){
		cut = wodEvGetTime();
		if(loop->preSec == 0){
			space = 0;
		}else{
			space = cut-loop->preSec;
		}
		loop->preSec = wodEvGetTime();
		_processTime(loop);
		_processIO(loop,space);
		_processIdle(loop);
	}
}
void wodEvStop(struct wodEvLoop *loop){
	loop->isQuit = 1;
}

int wodEvIOAdd(struct wodEvLoop *loop,int fd,int event,wodEvIOFn cb,void *cbArg){
	if(fd > loop->set_size || fd <= 0 || !cb){
		return -EINVAL;
	}
	struct wodEvIO *pFile = &loop->files[fd];
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
void wodEvIORemove(struct wodEvLoop *loop,int id,int event){
	if(id > loop->set_size){
		return ;
	}
	struct wodEvIO *pFile = &loop->files[id];
	if(pFile->event & event){
		loop->pollor.Remove(loop,pFile->fd,event);
		pFile->event &= (~event);
		if(pFile->event == WV_NONE){
			loop->used --;
		}
	}
}

int wodEvTimeAdd(struct wodEvLoop *loop,long long usec,wodEvTimeFn cb,void *cbArg){
	if(!cb){
		return -EINVAL;
	}
	struct wodEvTime * pTime = malloc(sizeof(struct wodEvTime));
	pTime->id = loop->idIndex++;
	pTime->cutClock = wodEvGetTime();
	pTime->sec = usec;
	pTime->passSec = 0.0;
	pTime->timeArg = cbArg;
	pTime->timeProc = cb;
	pTime->dispose = 0;
	int hash = _hashFunction(pTime->id);
	pTime->next = loop->hashMap[hash];
	loop->hashMap[hash] = pTime;
	return pTime->id;
}
void wodEvTimeRemove(struct wodEvLoop * loop,int id){
	int hash = _hashFunction(id);
	struct wodEvTime*tmp=loop->hashMap[hash];
	while(tmp){
		if(tmp->id == id){
			tmp->dispose = 1;
		}
		tmp = tmp->next;
	}
}

int wodEvUserDefAdd(struct wodEvLoop *loop,wodEvUserDefFn cb,void *cbArg){
	if(!cb){
		return -EINVAL;
	}
	struct wodEvUserDef * pIdle = malloc(sizeof(struct wodEvUserDef));
	pIdle->next = loop->userdefHead;
	loop->userdefHead  = pIdle;
	pIdle->id = loop->idIndex++;
	pIdle->userdefArg = cbArg;
	pIdle->idluceProc = cb;
	pIdle->dispose = 0;
	return pIdle->id;
}
void wodEvUserDefRemove(struct wodEvLoop *loop,int id){
	struct wodEvUserDef*tmp=loop->userdefHead;
	while(tmp){
		if(tmp->id == id){
			tmp->dispose = 1;
		}
		tmp = tmp->next;
	}
}

#define HAS_SELECT 1
#define HAS_POLL 1
#if HAS_EPOLL
#include "ev_epoll.c"
#endif
#if HAS_SELECT
#include "ev_select.c"
#endif
#if HAS_POLL
#include "ev_poll.c"
#endif
static int _initPollor(struct wodEvPoller* pllor,int type){
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
	}else if(type == WV_POLL_POLL){
#if HAS_POLL
		SET_POLLER(poller,poll);
		return WV_ROK;
#endif
		return -EINVAL;
	}
	return -EINVAL;
}
