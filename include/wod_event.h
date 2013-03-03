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
};
enum{
	WV_POLL_EPOLL,
	WV_POLL_SELECT,
	WV_POLL_POLL,
	WV_POLL_KQUEUE
};
typedef struct wod_event_main wod_event_main_t;

typedef void (*wod_event_io_fn)(wod_event_main_t *loop,void * nv,int mask);
typedef int (*wod_event_time_fn)(wod_event_main_t *loop,void * nv);
typedef int (*wod_event_userdef_fn)(wod_event_main_t *loop,void * nv);

wod_event_main_t * wod_event_main_new(int set_size,int type);
void wod_event_main_delete(wod_event_main_t *);
void wod_event_main_once(wod_event_main_t *);
void wod_event_main_loop(wod_event_main_t *);
void wod_event_main_stop(wod_event_main_t *);

int wod_event_io_add(wod_event_main_t *,int fd,int event,wod_event_io_fn cb,void *cbArg);
void wod_event_io_remove(wod_event_main_t *,int id,int event);
int wod_event_io_wait(int fd,int mask,long long timeout);
int wod_event_time_add(wod_event_main_t *,long long usec,wod_event_time_fn cb,void *cbArg);
void wod_event_time_remove(wod_event_main_t *,int id);
int wod_event_userdef_add(wod_event_main_t *,wod_event_userdef_fn cb,void *cbArg);
void wod_event_userdef_remove(wod_event_main_t *,int id);

#endif /* GODEV_H_ */
