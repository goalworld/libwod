/*
 * cycle_buffer.h
 *
 *  Created on: 2012-11-11
 *      Author: goalworld
 */

#ifndef _WOD_CYCLE_BUFFER_H_
#define _WOD_CYCLE_BUFFER_H_
#include <stdbool.h>
#include <stddef.h>
typedef struct wod_cycle_buffer
{
	char *buf;
	size_t cap;
	size_t head;
	size_t tail;
}wod_cycle_buffer_t;
typedef struct wod_cycle_pair
{
	struct {
		void *buf;
		size_t sz;
	}first,second;
}wod_cycle_pair_t;
int wod_cycle_buffer_init( wod_cycle_buffer_t* cycle,size_t defsize);
void wod_cycle_buffer_destroy( wod_cycle_buffer_t* cycle );
/*获取至少needsz的缓冲区大小 如果缓冲区大于needsz 将返回所有*/
int wod_cycle_buffer_get_unused(wod_cycle_buffer_t* cycle,wod_cycle_pair_t * pair,size_t needsz);
/*让使用的BUFFER增加 但不填充任何内容*/
int wod_cycle_buffer_grow(wod_cycle_buffer_t* cycle,size_t growsz);
/*让使用的BUFFER增加 而且在增加的那段位置填充BUF所指向的内容*/
int wod_cycle_buffer_push(wod_cycle_buffer_t* cycle,void *buf,size_t sz);
void wod_cycle_buffer_pop(wod_cycle_buffer_t* cycle,size_t sz);
size_t wod_cycle_buffer_used_size(wod_cycle_buffer_t* cycle);
int wod_cycle_buffer_get_used(wod_cycle_buffer_t* cycle,struct wod_cycle_pair * pair);
bool wod_cycle_buffer_empty( wod_cycle_buffer_t* cycle );

#define wod_cycle_pair_sz(pair) ( (pair)->first.sz + (pair)->second.sz )

int wod_cycle_pair_readsz(wod_cycle_pair_t * pair,void *buf,size_t sz);


#endif /* CYCLE_BUFFER_H_ */
