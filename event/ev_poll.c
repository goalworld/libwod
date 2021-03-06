#include "wod_evinner.h"
#include <poll.h>
#include <malloc.h>
#include <assert.h>
#include <errno.h>
//int poll(struct pollfd *fds, nfds_t nfds, int timeout);
/*
 struct pollfd {
               int   fd;        // file descriptor 
               short events;      //requested events 
               short revents;     //returned events 
           };
*/
typedef struct poll_data poll_data_t;
struct poll_data
{
	struct pollfd * pfdArr;
	int cap;
	int len;
};
static int 
_get_index(poll_data_t * pdata,int fd)
{
	int i=0;
	struct pollfd *cut;
	for(; i<pdata->len; i++){
		cut = pdata->pfdArr+i;
		if(cut->fd == fd){
			return i;
		}
	}
	return -1;
}
static void
_arr_push(poll_data_t *pdata,int fd,int events)
{
	if(pdata->len == pdata->cap){
		pdata->cap *=2;
		pdata->pfdArr = realloc(pdata->pfdArr,sizeof(struct pollfd) * pdata->cap);	
	}
	pdata->pfdArr[pdata->len].fd = fd;
	pdata->pfdArr[pdata->len].events = events;
	pdata->len++;
}
static int 
poll_new(struct wod_event * loop,int falg)
{
	poll_data_t * p = malloc(sizeof(poll_data_t));
	assert(p);
	p->pfdArr = malloc(sizeof(struct pollfd)*64);
	p->cap = 64;
	p->len = 0;
	loop->pollorData = p;
	return WOD_OK;
}
static void 
poll_delete(struct wod_event *loop)
{
	poll_data_t * p = ( poll_data_t *)loop->pollorData;
	free(p->pfdArr);
	free(p);
}
static int 
poll_add(struct wod_event *loop,int fd,int mask)
{
	
	poll_data_t * p = ( poll_data_t *)loop->pollorData;
	mask |=loop->files[fd].event;
	struct pollfd *cut;
	int ids = _get_index(p,fd);
	if( ids >=0 ){
		cut = p->pfdArr+ids;
		if( mask & WV_IO_READ )cut->events  |= POLLIN;
		if( mask & WV_IO_WRITE )cut->events |= POLLOUT;
	}else{
		int events = 0;
		if( mask & WV_IO_READ )events |= POLLIN;
		if( mask & WV_IO_WRITE )events |= POLLOUT;
		_arr_push(p,fd,events);
	}
	return WOD_OK;
}
static int 
poll_remove(struct wod_event *loop , int fd,int mask)
{
	poll_data_t * p = (poll_data_t *)loop->pollorData;
	mask =(loop->files[fd].event & (~mask));
	struct pollfd *cut;
	int ids = _get_index(p,fd);
	if(ids >=0 ){
		if(mask == WV_NONE){
			int sz = sizeof(struct pollfd)*(p->len-ids-1);
			if(sz){
				memmove(p->pfdArr+ids,p->pfdArr+ids+1,sz);
			}
			p->len --;
		}else{
			cut = p->pfdArr+ids;
			if( mask & WV_IO_READ )cut->events  |= POLLIN;
			if( mask & WV_IO_WRITE )cut->events |= POLLOUT;
		}
		return WOD_OK;
	}
	return -EINVAL;
}
static  int 
poll_poll(struct wod_event *loop,long long timeOut)
{
	poll_data_t * p = (poll_data_t *)loop->pollorData;
	
	int numelm = 0;
	struct wod_event_io * pio;
	struct pollfd * cut;
	int i = 0;
	int ret = poll(p->pfdArr,p->len, timeOut/1000);
	if(ret > 0){
		for(;i<p->len;i++){
			cut = p->pfdArr+i;
			pio = &loop->files[cut->fd];
			if( cut->revents ){
				pio->revent = cut->revents & ( POLLIN | POLLHUP | POLLERR ) ? WV_IO_READ:0
					|( cut->revents & ( POLLOUT | POLLHUP | POLLERR ) ) ? WV_IO_WRITE:0;
				loop->pendFds[numelm++] = cut->fd;
			}
		}
	}
	return ret < 0 ? -errno : numelm;	
}
