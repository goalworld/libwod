/*
 * wodEv.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef _WOD_WODEV_H_
#define _WOD_WODEV_H_

#include <stddef.h>
enum {
	WV_NONE=0X00,
	WV_IO_READ=0X01,
	WV_IO_WRITE=0X02
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
struct wodEvLoop;

typedef void (*wodEvIOFn)(struct wodEvLoop *loop,void * nv,int mask);
typedef int (*wodEvTimeFn)(struct wodEvLoop *loop,void * nv);
typedef int (*wodEvUserDefFn)(struct wodEvLoop *loop,void * nv);

struct wodEvLoop * wodEvLoopNew(int set_size,int type);
void wodEvLoopDel(struct wodEvLoop *);
void wodEvOnce(struct wodEvLoop *);
void wodEvRun(struct wodEvLoop *);
void wodEvStop(struct wodEvLoop *);

int wodEvIOAdd(struct wodEvLoop *,int fd,int event,wodEvIOFn cb,void *cbArg);
void wodEvIORemove(struct wodEvLoop *,int id,int event);
int wodEvIOWait(int fd,int mask,long long timeout);
int wodEvTimeAdd(struct wodEvLoop *,long long usec,wodEvTimeFn cb,void *cbArg);
void wodEvTimeRemove(struct wodEvLoop *,int id);
int wodEvUserDefAdd(struct wodEvLoop *,wodEvUserDefFn cb,void *cbArg);
void wodEvUserDefRemove(struct wodEvLoop *,int id);
long long wodEvGetTime();
void wodEvSleep(long long usec);
#endif /* GODEV_H_ */
