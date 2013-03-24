/*
 * wodEv.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef _WOD_WODEV_H_
#define _WOD_WODEV_H_

<<<<<<< HEAD
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
struct wod_event_main;

typedef void (*wod_event_io_fn)(struct wod_event_main *loop,void * nv,int mask);
typedef int (*wod_event_time_fn)(struct wod_event_main *loop,void * nv);
typedef int (*wod_event_userdef_fn)(struct wod_event_main *loop,void * nv);

struct wod_event_main * wod_event_main_new(int set_size,int type);
void wod_event_main_delete(struct wod_event_main *);
void wod_event_main_once(struct wod_event_main *);
void wod_event_main_loop(struct wod_event_main *);
void wod_event_main_stop(struct wod_event_main *);

int wod_event_io_add(struct wod_event_main *,int fd,int event,wod_event_io_fn cb,void *cbArg);
void wod_event_io_remove(struct wod_event_main *,int id,int event);
int wod_event_io_wait(int fd,int mask,long long timeout);
int wod_event_time_add(struct wod_event_main *,long long usec,wod_event_time_fn cb,void *cbArg);
void wod_event_time_remove(struct wod_event_main *,int id);
=======
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

int wod_event_userdef_add(wod_event_t *,wod_event_userdef_fn cb,void *cbArg);
void wod_event_userdef_remove(wod_event_t *,int id);
>>>>>>> 58eb921e629e090962a4022f193227ae241864b3

#endif /* GODEV_H_ */
