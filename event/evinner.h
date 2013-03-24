/*
 * wodEv_inner.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef WODEV_INNER_H_
#define WODEV_INNER_H_
#include "../include/wod_event.h"
#define HASH_SIZE 32
#define SLEEP 10000000
struct wod_event_main;
struct wod_event_pollor
{
	int (*new)		(struct wod_event_main * loop,int flag);
	void (*delete)	(struct wod_event_main *loop);
	int (*add)		(struct wod_event_main *loop,int fd,int mask);
	int	(*remove)	(struct wod_event_main *loop,int fd,int mask);
	int (*poll)		(struct wod_event_main *loop,long long timeOut);
};
struct wod_event_time
{
	int id;
	wod_event_time_fn timeProc;
	void * timeArg;
	long long sec;
	long long passSec;
	long long cutClock;
	int dispose;
	int repetTims;
	struct wod_event_time *next;
};
struct wod_event_io
{
	int fd;
	wod_event_io_fn readProc;
	void * readArg;
	wod_event_io_fn writeProc;
	void * writeArg;
	int event;//WV_NONE
	int revent;//WV_NONE
};
struct wod_event_main
{
	struct wod_event_pollor pollor;
	void * pollorData;
	int isQuit;
	int set_size;
	struct wod_event_io * files;
	int *pendFds;
	int idIndex;
	int used;
	struct wod_event_time* hashMap[HASH_SIZE];
	long long minSec;
	long long preSec;
};
#define SET_POLLER(poller,type) \
				pllor->new = type##_new;\
				pllor->delete = type##_delete;\
				pllor->add = type##_add;\
				pllor->remove = type##_remove;\
				pllor->poll = type##_poll;

#endif /* GODEV_INNER_H_ */
