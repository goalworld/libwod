/*
 * cycle_buffer.h
 *
 *  Created on: 2012-11-11
 *      Author: goalworld
 */

#ifndef _WOD_CYCLE_BUFFER_H_
#define _WOD_CYCLE_BUFFER_H_
#include <stdbool.h>
typedef struct wod_cycle_buffer
{
	char *buf;
	int cap;
	int head;
	int tail;
}wod_cycle_buffer_t;
typedef struct wod_cycle_pair
{
	struct {
		void *buf;
		int sz;
	}first,second;
}wod_cycle_pair_t;
int wod_cycle_buffer_init( wod_cycle_buffer_t* cycle,int defsize);
void wod_cycle_buffer_destroy( wod_cycle_buffer_t* cycle );
int wod_cycle_buffer_grow(wod_cycle_buffer_t* cycle,int growsz,struct wod_cycle_pair *pair);
void wod_cycle_buffer_back(wod_cycle_buffer_t* cycle,int backsz);
int wod_cycle_buffer_push(wod_cycle_buffer_t* cycle,void *buf,int sz);
void wod_cycle_buffer_pop(wod_cycle_buffer_t* cycle,int sz);
int wod_cycle_buffer_get_used(wod_cycle_buffer_t* cycle,struct wod_cycle_pair * pair);
bool wod_cycle_buffer_empty( wod_cycle_buffer_t* cycle );
#endif /* CYCLE_BUFFER_H_ */
