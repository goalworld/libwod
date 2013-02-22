/*
 * rain_queue.h
 *
 *  Created on: 2013-1-17
 *      Author: wd
 */

#ifndef RAIN_QUEUE_H_
#define RAIN_QUEUE_H_

struct wcQueue
{
	void * q_buf;
	int q_cut;
	int q_end;
	int q_len;
	int q_elemsize;
};
typedef void wcQueueElemDelFn(void *elem);
int 		wcQueueInit		(struct wcQueue *que, unsigned elemsize);
void 		wcQueueDestroy	(struct wcQueue *que, wcQueueElemDelFn fn);
void 		wcQueuePush		(struct wcQueue *que, void *elem);
int 		wcQueuePop		(struct wcQueue *que, void *elem);
unsigned 	wcQueueSize		(struct wcQueue *que );
#endif /* RAIN_QUEUE_H_ */
