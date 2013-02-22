/*
 * wodEv_inner.h
 *
 *  Created on: 2012-10-23
 *      Author: goalworld
 */

#ifndef WODEV_INNER_H_
#define WODEV_INNER_H_
#include "../include/wod_ev.h"
#define HASH_SIZE 32
#define SLEEP 1E1//1ms
struct wodEvLoop;
struct wodEvPoller{
	int (*New)(struct wodEvLoop * loop,int flag);
	void (*Del)(struct wodEvLoop *loop);
	int (*Add)(struct wodEvLoop *loop,int fd,int mask);
	int	(*Remove)(struct wodEvLoop *loop , int fd,int mask);
	int (*Poll)(struct wodEvLoop *loop,double timeOut);
};
struct wodEvTime{
	int id;
	wodEvTimeFn timeProc;
	void * timeArg;
	double sec;
	double passSec;
	double cutClock;
	int dispose;
	int repetTims;
	struct wodEvTime *next;
};
struct wodEvIO{
	int fd;
	wodEvIOFn readProc;
	void * readArg;
	wodEvIOFn writeProc;
	void * writeArg;
	int event;//WV_NONE
	int revent;//WV_NONE
};
struct wodEvUserDef{
	int id;
	wodEvUserDefFn idluceProc;
	int dispose;
	void * userdefArg;
	struct wodEvUserDef * next;
};
struct wodEvLoop{
	struct wodEvPoller pollor;
	void * pollorData;
	int isQuit;
	int set_size;
	struct wodEvIO * files;
	int *pendFds;
	int idIndex;
	struct wodEvUserDef * userdefHead;
	struct wodEvTime* hashMap[HASH_SIZE];
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
