/*
 * wv_inner.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef WODEV_INNER_H_
#define WODEV_INNER_H_
#include "ev.h"
#define HASH_SIZE 32
#define SLEEP 1E1//1ms
struct wvLoop;
struct wvPoller{
	int (*New)(struct wvLoop * loop,int flag);
	void (*Del)(struct wvLoop *loop);
	int (*Add)(struct wvLoop *loop,int fd,int mask);
	int	(*Remove)(struct wvLoop *loop , int fd,int mask);
	int (*Poll)(struct wvLoop *loop,double timeOut);
};
struct wvTime{
	int id;
	wvTimeFn timeProc;
	void * timeArg;
	double sec;
	double passSec;
	double cutClock;
	int dispose;
	int repetTims;
	struct wvTime *next;
};
struct wvIO{
	int fd;
	wvIOFn readProc;
	void * readArg;
	wvIOFn writeProc;
	void * writeArg;
	int event;//WV_NONE
	int revent;//WV_NONE
};
struct wvUserDef{
	int id;
	wvUserDefFn idluceProc;
	int dispose;
	void * userdefArg;
	struct wvUserDef * next;
};
struct wvLoop{
	struct wvPoller pollor;
	void * pollorData;
	int isQuit;
	int set_size;
	struct wvIO * files;
	int *pendFds;
	int idIndex;
	struct wvUserDef * userdefHead;
	struct wvTime* hashMap[HASH_SIZE];
	int used;
	double minSec;
};
#define SET_POLLER(poller,type) \
				pllor->Add = type##Add;\
				pllor->Del = type##Del;\
				pllor->New = type##New;\
				pllor->Poll = type##Poll;\
				pllor->Remove = type##Remove;
#endif /* GODEV_INNER_H_ */
