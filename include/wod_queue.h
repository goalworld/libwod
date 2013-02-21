/*
 * wod_queue.h
 *
 *  Created on: 2013-1-17
 *      Author: wd
 */

#ifndef _WOD__QUEUE_H_
#define _WOD__QUEUE_H_

struct wod_queue
{
	void * q_buf;
	int q_cut;
	int q_end;
	int q_len;
	int q_elemsize;
};
typedef void wod_queue_elem_del_fn(void *elem);
int 		wod_queue_init		(struct wod_queue *que, unsigned elemsize);
void 		wod_queue_destroy	(struct wod_queue *que, wod_queue_elem_del_fn fn);
void 		wod_queue_push		(struct wod_queue *que, void *elem);
int 		wod_queue_pop		(struct wod_queue *que, void *elem);
unsigned 	wod_queue_size		(struct wod_queue *que );
#endif /* RAIN_QUEUE_H_ */
