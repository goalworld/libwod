/*
 * rain_queue.h
 *
 *  Created on: 2013-1-17
 *      Author: wd
 */

#ifndef RAIN_QUEUE_H_
#define RAIN_QUEUE_H_

typedef struct wcQueue
{
	void * q_buf;
	int q_cut;
	int q_end;
	int q_len;
	int q_elemsize;
}wcQueue;
typedef void wcQueueElemDelFn(void *elem);
int 		wcQueueInit		(wcQueue *que, unsigned elemsize);
void 		wcQueueDestroy	(wcQueue *que, wcQueueElemDelFn fn);
void 		wcQueuePush		(wcQueue *que, void *elem);
int 		wcQueuePop		(wcQueue *que, void *elem);
unsigned 	wcQueueSize		(wcQueue *que );
#endif /* RAIN_QUEUE_H_ */
