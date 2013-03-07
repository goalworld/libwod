/*
 * ev_select.h
 *
 *  Created on: 2013-2-4
 *      Author: Administrator
 */

#include "wod_evinner.h"
#include <errno.h>
#include <sys/select.h>
#include <assert.h>
typedef struct selete_data{
	int maxfd;
	fd_set rset;
	fd_set wset;
}selete_data_t;
static int
select_new(wod_event_t * loop,int flag)
{
	selete_data_t * p = malloc(sizeof(selete_data_t));
	assert(p);
	p->maxfd = 0;
	FD_ZERO(&p->rset);
	FD_ZERO(&p->wset);
	loop->pollorData = p;
	return WOD_OK;
}
static void
select_delete(wod_event_t *loop)
{
	free(loop->pollorData);
	return ;
}
static int
select_add(wod_event_t *loop,int fd,int mask)
{
	selete_data_t * p = (selete_data_t *)loop->pollorData;
	if(fd > p->maxfd){
		p->maxfd = fd;
	}
	mask |=loop->files[fd].event;
	FD_CLR(fd,&p->rset);
	FD_CLR(fd,&p->wset);
	if( mask & WV_IO_READ )FD_SET(fd,&p->rset);
	if( mask & WV_IO_WRITE )FD_SET(fd,&p->wset);
	return WOD_OK;
}
static int
select_remove(wod_event_t *loop , int fd,int mask)
{
	selete_data_t * p = (selete_data_t *)loop->pollorData;
	mask =(loop->files[fd].event & (~mask));
	FD_CLR(fd,&p->rset);
	FD_CLR(fd,&p->rset);
	if( mask & WV_IO_READ )FD_SET(fd,&p->rset);
	if( mask & WV_IO_WRITE )FD_SET(fd,&p->wset);
	if( mask == WV_NONE && fd == p->maxfd){
		int i = p->maxfd;
		for(;i >=0 ;i--){
			struct wod_event_io * pio = &loop->files[i];
			if(pio->event != WV_NONE) p->maxfd = pio->fd;
		}
	}
	return WOD_OK;
}
static int
select_poll(wod_event_t *loop,long long timeOut)
{
	selete_data_t * p = (selete_data_t *)loop->pollorData;
	fd_set rset,wset;
	rset = p->rset;
	wset = p->wset;
	int max = p->maxfd + 1,i=0,numele=0;
	struct timeval tv;
	tv.tv_sec = timeOut/1000000;
	tv.tv_usec = (timeOut - tv.tv_sec);
	int ret = select(max,&rset,&wset,NULL,&tv);
	switch (ret){
	case 0:return 0;
	case -1:return -errno;
	}
	for(;i < max;i++){
		wod_event_io_t * pio = &loop->files[i];
		if(pio->event == WV_NONE) continue;
		pio->revent = WV_NONE;
		if( (pio->event & WV_IO_READ) && FD_ISSET(pio->fd,&rset))	pio->revent |= WV_IO_READ;
		if((pio->event& WV_IO_WRITE) && FD_ISSET(pio->fd,&wset))	pio->revent |= WV_IO_WRITE;
		if(pio->revent != WV_NONE) 		loop->pendFds[numele++] = pio->fd;
	}
	return numele;
}
