/*
 * ev_select.h
 *
 *  Created on: 2013-2-4
 *      Author: Administrator
 */

#include "evinner.h"
#include <errno.h>
#include <sys/select.h>
#include <assert.h>
typedef struct selectData{
	int maxfd;
	fd_set rset;
	fd_set wset;
}selectData;
static int
selectNew(struct wodEvLoop * loop,int flag)
{
	selectData * p = malloc(sizeof(selectData));
	assert(p);
	p->maxfd = 0;
	FD_ZERO(&p->rset);
	FD_ZERO(&p->wset);
	loop->pollorData = p;
	return WV_ROK;
}
static void
selectDel(struct wodEvLoop *loop)
{
	free(loop->pollorData);
	return ;
}
static int
selectAdd(struct wodEvLoop *loop,int fd,int mask)
{
	selectData * p = (selectData *)loop->pollorData;
	if(fd > p->maxfd){
		p->maxfd = fd;
	}
	mask |=loop->files[fd].event;
	FD_CLR(fd,&p->rset);
	FD_CLR(fd,&p->wset);
	if( mask & WV_IO_READ )FD_SET(fd,&p->rset);
	if( mask & WV_IO_WRITE )FD_SET(fd,&p->wset);
	return WV_ROK;
}
static int
selectRemove(struct wodEvLoop *loop , int fd,int mask)
{
	selectData * p = (selectData *)loop->pollorData;
	mask =(loop->files[fd].event & (~mask));
	FD_CLR(fd,&p->rset);
	FD_CLR(fd,&p->rset);
	if( mask & WV_IO_READ )FD_SET(fd,&p->rset);
	if( mask & WV_IO_WRITE )FD_SET(fd,&p->wset);
	if( mask == WV_NONE && fd == p->maxfd){
		int i = p->maxfd;
		for(;i >=0 ;i--){
			struct wodEvIO * pio = &loop->files[i];
			if(pio->event != WV_NONE) p->maxfd = pio->fd;
		}
	}
	return WV_ROK;
}
static int
selectPoll(struct wodEvLoop *loop,long long timeOut)
{
	selectData * p = (selectData *)loop->pollorData;
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
		struct wodEvIO * pio = &loop->files[i];
		if(pio->event == WV_NONE) continue;
		pio->revent = WV_NONE;
		if(pio->event& WV_IO_READ && FD_ISSET(pio->fd,&rset))	pio->revent |= WV_IO_READ;
		if(pio->event& WV_IO_WRITE && FD_ISSET(pio->fd,&wset))	pio->revent |= WV_IO_WRITE;
		if(pio->revent != WV_NONE) 		loop->pendFds[numele++] = pio->fd;
	}
	return numele;
}
