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
selectNew(struct wvLoop * loop,int flag)
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
selectDel(struct wvLoop *loop)
{
	free(loop->pollorData);
	return ;
}
static int
selectAdd(struct wvLoop *loop,int fd,int mask)
{

	selectData * p = (selectData *)loop->pollorData;
	if(fd > p->maxfd){
		p->maxfd = fd;
	}
	mask |=loop->files[fd].event;
	FD_CLR(fd,&p->rset);
	FD_CLR(fd,&p->rset);
	if( mask & WV_IO_READ )FD_SET(fd,&p->rset);
	if( mask & WV_IO_WRITE )FD_SET(fd,&p->wset);
	return WV_ROK;
}
static int
selectRemove(struct wvLoop *loop , int fd,int mask)
{
	selectData * p = (selectData *)loop->pollorData;
	mask =(loop->files[fd].event & (~mask));
	FD_CLR(fd,&p->rset);
	FD_CLR(fd,&p->rset);
	if( mask & WV_IO_READ )FD_SET(fd,&p->rset);
	if( mask & WV_IO_WRITE )FD_SET(fd,&p->wset);
	if( mask == WV_NONE){
		if(fd == p->maxfd){
			p->maxfd = fd;
		}
	}
	return WV_ROK;
}
static int
selectPoll(struct wvLoop *loop,double timeOut)
{
	selectData * p = (selectData *)loop->pollorData;
	fd_set rset,wset;
	rset = p->rset;
	wset = p->wset;
	struct timeval tv;
	tv.tv_sec = timeOut;
	tv.tv_usec = (timeOut - tv.tv_sec) * 1000000;
	int ret = select(p->maxfd,&rset,&wset,NULL,&tv);
	switch (ret){
	case 0:
		return 0;
	case -1:
		return -errno;
	default:
		break;
	}
	int i = 0;
	int * pPendFd = loop->pendFds;
	for(;i < loop->set_size;i++){
		struct wvIO * pio = &loop->files[i];
		if(pio->event != WV_NONE){
			pio->revent = WV_NONE;
			if(FD_ISSET(pio->fd,&rset))pio->revent |= WV_IO_READ;
			if(FD_ISSET(pio->fd,&wset))pio->revent |= WV_IO_WRITE;
			*pPendFd = pio->fd;pPendFd ++;
		}
	}
	return ret;
}
