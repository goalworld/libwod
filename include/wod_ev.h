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
#define WV_NONE 0X00
	WV_IO_READ=0X01,
#define WV_IO_READ 0X01
	WV_IO_WRITE=0X02
#define WV_IO_WRITE 0X01
};
enum{
	WV_ROK = 0
#define WV_ROK 0X0
};
enum{
	WV_POLL_EPOLL,
	WV_POLL_SELECT,
	WV_POLL_POLL,
	WV_POLL_KQUEUE
};
struct wvLoop;

typedef void (*wvIOFn)(struct wvLoop *loop,void * nv,int mask);
typedef int (*wvTimeFn)(struct wvLoop *loop,void * nv);
typedef int (*wvUserDefFn)(struct wvLoop *loop,void * nv);

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