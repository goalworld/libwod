/*
 * wv.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef WODEV_H_
#define WODEV_H_

#include <stddef.h>
enum {
	WV_NONE=0X00,
	WV_IO_READ=0X01,
	WV_IO_WRITE=0X02
};
enum{
	WV_ROK = 0,
};
enum{
	WV_POLL_EPOLL,
	WV_POLL_SELECT,
	WV_POLL_KQUEUE
};
typedef void (*wvIOFn)(void * nv,int mask);
typedef int (*wvTimeFn)(void * nv);
typedef int (*wvUserDefFn)(void * nv);

struct wvLoop;
struct wvLoop * wvLoopNew(int set_size,int type);
void wvLoopDel(struct wvLoop *);
void wvRun(struct wvLoop *);
void wvStop(struct wvLoop *);

int wvIOAdd(struct wvLoop *,int fd,int event,wvIOFn cb,void *cbArg);
void wvIORemove(struct wvLoop *,int id,int event);
int wvIOWait(int fd,int mask,long long timeout);
int wvTimeAdd(struct wvLoop *,int sec,int usec,wvTimeFn cb,void *cbArg);
void wvTimeRemove(struct wvLoop *,int id);
int wvUserDefAdd(struct wvLoop *,wvUserDefFn cb,void *cbArg);
void wvUserDefRemove(struct wvLoop *,int id);

#endif /* GODEV_H_ */
