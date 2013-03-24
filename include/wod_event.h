/*
 * wodEv.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef _WOD_WODEV_H_
#define _WOD_WODEV_H_

#include "wod_type.h"

#define WV_NONE 		0X00
#define WV_IO_READ 		0X01
#define WV_IO_WRITE 	0X02
#define WV_POLL_EPOLL 	1
#define	WV_POLL_SELECT 	2
#define	WV_POLL_POLL 	3
#define	WV_POLL_KQUEUE 	4

typedef struct wod_event wod_event_t;

typedef void (*wod_event_io_fn)(wod_event_t *loop,void * nv,int mask);
typedef int (*wod_event_time_fn)(wod_event_t *loop,void * nv);
typedef int (*wod_event_userdef_fn)(wod_event_t *loop,void * nv);

wod_ret_t wod_event_create(wod_event_t **ev,int set_size,int type);
void wod_event_destroy(wod_event_t *);
void wod_event_once(wod_event_t *);
void wod_event_loop(wod_event_t *);
void wod_event_stop(wod_event_t *);

int wod_event_io_add(wod_event_t *,int fd,int event,wod_event_io_fn cb,void *cbArg);
void wod_event_io_remove(wod_event_t *,int id,int event);

int wod_event_time_add(wod_event_t *,long long usec,wod_event_time_fn cb,void *cbArg);
void wod_event_time_remove(wod_event_t *,int id);

#endif /* GODEV_H_ */
