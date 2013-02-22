/*
 * rain_queue.c
 *
 *  Created on: 2013-1-17
 *      Author: wd
 */


#include "wod_queue.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#define BUF_SZ 64
int
wcQueueInit(struct wcQueue *que,unsigned elemsize)
{
	assert(que);
	que->q_buf = malloc(BUF_SZ * elemsize);
	if(que->q_buf == NULL){
		return -1;
	}
	que->q_cut = 0;
	que->q_end = 0;
	que->q_elemsize = elemsize;
	que->q_len = BUF_SZ;
	return 0;
}
void
wcQueueDestroy(struct wcQueue *que,wcQueueElemDelFn del_fn){
	assert(que);
	if(del_fn){
		int cut = que->q_cut;
		while(cut != que->q_end){
			del_fn((uint8_t*)que->q_buf + cut*que->q_elemsize);
			cut = (cut+1)%que->q_len;
		}
	}
	free(que->q_buf);

}
void
wcQueuePush(struct wcQueue * que,void *elem)
{
	memcpy((uint8_t*)que->q_buf + que->q_end*que->q_elemsize,elem,que->q_elemsize );
	que->q_end = (que->q_end+1)%que->q_len;
	if(que->q_cut == que->q_end){
		int newsz = que->q_len * 2;
		void * newbuf = malloc(newsz * que->q_elemsize);
		int firstlen = que->q_len - que->q_cut;
		memcpy( newbuf,
				(uint8_t *)que->q_buf + que->q_cut * que->q_elemsize,
				firstlen*que->q_elemsize);
		memcpy( (uint8_t *)newbuf + firstlen*que->q_elemsize,
				que->q_buf,
				que->q_end*que->q_elemsize);
		que->q_cut = 0;
		que->q_end = que->q_len;
		que->q_len = newsz;
		free(que->q_buf);
		que->q_buf = newbuf;
	}
}
int
wcQueuePop(struct wcQueue * que,void *elem)
{
	assert(que && elem);
	if(que->q_cut == que->q_end){
		return -1;
	}
	memcpy(elem,(uint8_t*)que->q_buf + que->q_cut*que->q_elemsize,que->q_elemsize);
	que->q_cut = (que->q_cut + 1)%que->q_len;
	return 0;
}
unsigned
wcQueueSize( struct wcQueue * que )
{
	if(que->q_cut <= que->q_end){
		return que->q_end - que->q_cut;
	}else{
		return que->q_cut + que->q_len - que->q_end-1;
	}
}

