#include "evinner.h"
#include <poll.h>
#include <malloc.h>
#include <assert.h>
#include <wod/array.h>
#include <errno.h>
//int poll(struct pollfd *fds, nfds_t nfds, int timeout);
/*
 struct pollfd {
               int   fd;        // file descriptor 
               short events;      //requested events 
               short revents;     //returned events 
           };
*/
struct pollData
{
	wcArray arr;
};
static int 
_getIndex(struct pollData * pdata,int fd)
{
	int sz = wcArraySize(&pdata->arr),i=0;
	struct pollfd cut;
	for(; i<sz; i++){
		wcArrayAt( &pdata->arr,i,&cut);
		if(cut.fd == fd){
			return i;
		}
	}
	return -1;
}
static int 
pollNew(struct wvLoop * loop,int falg)
{
	struct pollData * p = malloc(sizeof(struct pollData));
	assert(p);
	wcArrayInit(&p->arr,sizeof(struct pollfd));
	loop->pollorData = p;
	return WV_ROK;
}
static void 
pollDel(struct wvLoop *loop)
{
	free(loop->pollorData);
}
static int 
pollAdd(struct wvLoop *loop,int fd,int mask)
{
	
	struct pollData * p = ( struct pollData *)loop->pollorData;
	mask |=loop->files[fd].event;
	struct pollfd cut;
	int ids = _getIndex(p,fd);
	if( ids >=0 ){
		wcArrayAt( &p->arr,ids,&cut);
		if( mask & WV_IO_READ )cut.events  |= POLLIN;
		if( mask & WV_IO_WRITE )cut.events |= POLLOUT;
		wcArraySet( &p->arr,ids,&cut);
	}else{
		cut.fd =fd;
		cut.events = 0;
		if( mask & WV_IO_READ )cut.events |= POLLIN;
		if( mask & WV_IO_WRITE )cut.events |= POLLOUT;
		wcArrayPush(&p->arr,&cut);
	}

	return WV_ROK;
}
static int 
pollRemove(struct wvLoop *loop , int fd,int mask)
{
	struct pollData * p = (struct pollData *)loop->pollorData;
	mask =(loop->files[fd].event & (~mask));
	struct pollfd cut;
	int ids = _getIndex(p,fd);
	if(ids >=0 ){
		if(mask == WV_NONE){
			wcArrayErase(&p->arr,ids,1,NULL);
		}else{
			wcArrayAt( &p->arr,ids,&cut);
			if( mask & WV_IO_READ )cut.events |= POLLIN;
			if( mask & WV_IO_WRITE )cut.events |= POLLOUT;
			wcArraySet( &p->arr,ids,&cut);
		}
		return WV_ROK;
	}
	return -EINVAL;
}
static  int 
pollPoll(struct wvLoop *loop,double timeOut)
{
	struct pollData * p = (struct pollData *)loop->pollorData;

	int sz = wcArraySize(&p->arr);
	struct pollfd pfds[sz];
	memcpy( pfds,wcArrayGetBuffer(&p->arr),sizeof(struct pollfd)*sz);

	int ret = poll(pfds,sz, timeOut*1E3);
	int numelm = 0;
	if(ret > 0){
		struct wvIO * pio;
		for(;sz>0;sz--){
			pio = &loop->files[pfds[sz-1].fd];
			if( pfds[sz-1].revents ){
				pio->revent = pfds[sz-1].revents & ( POLLIN | POLLHUP | POLLERR ) ? WV_IO_READ:0
					|( pfds[sz-1].revents & ( POLLOUT | POLLHUP | POLLERR ) ) ? WV_IO_WRITE:0;
				loop->pendFds[numelm++] = pfds[sz-1].fd;
			}
		}
	}
	return ret < 0 ? -errno : numelm;	
}
