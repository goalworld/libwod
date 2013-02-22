/*
 * wod_queue.h
 *
 *  Created on: 2013-1-17
 *      Author: wd
 */

#ifndef _WOD__QUEUE_H_
#define _WOD__QUEUE_H_

struct wodQueue
{
	void * q_buf;
	int q_cut;
	int q_end;
	int q_len;
	int q_elemsize;
};
typedef void wodQueueElemDelFn(void *elem);
int 		wodQueueInit		(struct wodQueue *que, unsigned elemsize);
void 		wodQueueDestroy	(struct wodQueue *que, wodQueueElemDelFn fn);
void 		wodQueuePush		(struct wodQueue *que, void *elem);
int 		wodQueuePop		(struct wodQueue *que, void *elem);
unsigned 	wodQueueSize		(struct wodQueue *que );
#endif /* RAIN_QUEUE_H_ */
