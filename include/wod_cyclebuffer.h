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
	void *cutbuf;
	struct {
		void *buf;
		size_t sz;
	}first,second;
}wod_cycle_pair_t;
int wod_cycle_buffer_init( wod_cycle_buffer_t* cycle,size_t defsize);
void wod_cycle_buffer_destroy( wod_cycle_buffer_t* cycle );
int wod_cycle_buffer_grow(wod_cycle_buffer_t* cycle,size_t growsz,struct wod_cycle_pair *pair);
void wod_cycle_buffer_back(wod_cycle_buffer_t* cycle,size_t backsz);
int wod_cycle_buffer_push(wod_cycle_buffer_t* cycle,void *buf,size_t sz);
void wod_cycle_buffer_pop(wod_cycle_buffer_t* cycle,size_t sz);
int wod_cycle_buffer_get_used(wod_cycle_buffer_t* cycle,struct wod_cycle_pair * pair);
bool wod_cycle_buffer_empty( wod_cycle_buffer_t* cycle );

#define wod_cycle_pair_sz(pair) (pair)->first.sz + (pair)->second.sz;

int wod_cycle_pair_readsz(wod_cycle_pair_t * pair,void *buf,size_t sz);


#endif /* CYCLE_BUFFER_H_ */
