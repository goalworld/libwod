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
	int * fdmap;
};

int 
pollNew(struct wvLoop * loop,int falg)
{
	struct pollData * p = malloc(sizeof(struct pollData));
	assert(p);
	p->fdmap = (int*)malloc(sizeof(int)*(loop->set_size));
	assert(p->fdmap);
	int i = 0;
	for(;i<loop->set_size;i++){
		p->fdmap[i] = -1;
	}
	//memset(&p.fdmap,-1,sizeof(int)*loop->set_size)
	wcArrayInit(&p->arr,sizeof(struct pollfd));
	loop->pollorData = p;
	return WV_ROK;
}
void 
pollDel(struct wvLoop *loop)
{
	free(loop->pollorData);
}
int 
pollAdd(struct wvLoop *loop,int fd,int mask)
{
	
	struct pollData * p = ( struct pollData *)loop->pollorData;
	mask |=loop->files[fd].event;
	struct pollfd cut;
	int ids = p->fdmap[fd];
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
		p->fdmap[fd] = wcArraySize(&p->arr) - 1;
	}

	return WV_ROK;
}
int 
pollRemove(struct wvLoop *loop , int fd,int mask)
{
	struct pollData * p = (struct pollData *)loop->pollorData;
	mask =(loop->files[fd].event & (~mask));
	struct pollfd cut;
	int ids = p->fdmap[fd];
	if(ids >=0 ){
		if(mask == WV_NONE){
			wcArrayErase(&p->arr,ids,1,NULL);
			p->fdmap[fd] = -1;
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
int 
pollPoll(struct wvLoop *loop,double timeOut)
{
	struct pollData * p = (struct pollData *)loop->pollorData;

	int sz = wcArraySize(&p->arr);
	struct pollfd pfds[sz];
	memcpy( pfds,wcArrayGetBuffer(&p->arr),sizeof(struct pollfd)*sz);

	int ret = poll(pfds,sz, timeOut*1E3);
	printf(" pollPoll %d %d %d %d\n",ret,sz,pfds[0].fd,pfds[0].events );
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
