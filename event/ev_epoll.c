/*
 * gev_epoll.c
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#include "wod_evinner.h"
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
typedef struct wod_epoll_data{
	int epFd;
}wod_epoll_data_t;

static int 
epoll_new(wod_event_t * loop,int flag)
{
	wod_epoll_data_t * pInfo = malloc(sizeof(wod_epoll_data_t));
	if((pInfo->epFd = epoll_create1(EPOLL_CLOEXEC)) < 0){
		free(pInfo);
		return -errno;
	}
	loop->pollorData = pInfo;
	return WOD_OK;
}
static void 
epoll_delete(wod_event_t *loop)
{
	wod_epoll_data_t * pInfo = (wod_epoll_data_t *)(loop->pollorData);
	close( pInfo->epFd );
	free(loop->pollorData);
}
static int 
epoll_add(wod_event_t *loop, int fd, int mask)
{
	wod_epoll_data_t * pInfo = (wod_epoll_data_t *)(loop->pollorData);
	struct epoll_event epEv;
	epEv.data.fd = fd;
	mask |=loop->files[fd].event;
	epEv.events = (mask & WV_IO_READ) ? EPOLLIN:0 |(mask & WV_IO_WRITE) ? EPOLLOUT:0 ;
	int ret = -1;
	if(epEv.events != 0){
		//epEv.events |= EPOLLET
		
		if(loop->files[fd].event == WV_NONE){
			ret = epoll_ctl(pInfo->epFd,EPOLL_CTL_ADD,fd,&epEv);
			if( ret < 0 && errno == EEXIST){
				ret = epoll_ctl(pInfo->epFd,EPOLL_CTL_MOD,fd,&epEv);
			}
		}else{
			ret = epoll_ctl(pInfo->epFd,EPOLL_CTL_MOD,fd,&epEv);
		}
	}
	return ret >=0 ? WOD_OK : -errno ;
}

static int 
epoll_remove(wod_event_t *loop ,int fd, int mask)
{
	wod_epoll_data_t * pInfo = (wod_epoll_data_t *)(loop->pollorData);
	struct epoll_event epEv;
	epEv.data.fd = fd;
	mask =(loop->files[fd].event & (~mask));
	epEv.events = (mask & WV_IO_READ) ? EPOLLIN:0 |(mask & WV_IO_WRITE) ? EPOLLOUT:0 ;
	if(mask == WV_NONE){
		return epoll_ctl(pInfo->epFd,EPOLL_CTL_DEL,fd,&epEv);
	}else{
		return epoll_ctl(pInfo->epFd,EPOLL_CTL_MOD,fd,&epEv);
	}
}

static int 
epoll_poll(wod_event_t *loop,long long timeOut)
{
	wod_epoll_data_t * pInfo = (wod_epoll_data_t *)(loop->pollorData);
	int ret;
	struct epoll_event epEvs[loop->set_size];
	while(1){
		 ret = epoll_wait(pInfo->epFd,epEvs,loop->set_size,timeOut/1000);
		 if(ret < 0 && (errno == EINTR)) continue;
		 else break;

	}
	if(ret > 0){
		int tmp = ret;
		struct epoll_event * epEv = epEvs ;
		int * pPendFd = loop->pendFds;
		for(;tmp>0;tmp--){
			loop->files[epEv->data.fd].revent =
			( ( epEv->events & (EPOLLIN | EPOLLHUP | EPOLLERR) ) > 0 ? WV_IO_READ:0 )
			*(pPendFd++) = epEv->data.fd;
			epEv++;
		}
	}
	return ret;
}
