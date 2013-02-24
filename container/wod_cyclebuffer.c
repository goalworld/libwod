/*
 * cycle_buffer.c
 *
 *  Created on: 2012-11-11
 *      Author: goalworld
 */
#include "../include/wod_cyclebuffer.h"
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
static int _resize_byuse(wod_cycle_buffer_t* cycle,int usesz);
static int 	_get_free_sz(wod_cycle_buffer_t* cycle);
int
wod_cycle_buffer_init( wod_cycle_buffer_t* cycle,int defsize)
{
	if(!cycle || defsize <= 0){
		return -1;
	}
	cycle->buf = malloc(defsize);
	cycle->cap = defsize;
	cycle->head = 0;
	cycle->tail = 0;
	return 0;
}
int
wod_cycle_buffer_push(wod_cycle_buffer_t* cycle,void *buf,int bufsz)
{
	if(!buf || bufsz<=0){
		return -1;
	}
	if( !_resize_byuse(cycle,bufsz)){
		return -1;
	}
	if(cycle->head > cycle->tail){
		memcpy(cycle->buf+cycle->tail,buf,bufsz);
		cycle->tail+=bufsz;
	}else{
		int efz = cycle->cap - cycle->tail;
		if(bufsz > efz){
			memcpy(cycle->buf+cycle->tail,buf,efz);
			memcpy(cycle->buf,buf+efz,bufsz-efz);
			cycle->tail = bufsz-efz;
		}else{
			memcpy(cycle->buf+cycle->tail,buf,bufsz);
			cycle->tail+=bufsz;
		}
	}
	return 0;
}

void
wod_cycle_buffer_pop(wod_cycle_buffer_t* cycle,int sz)
{
	int usedsz = cycle->cap - _get_free_sz(cycle);
	if(usedsz > sz){
		cycle->head = (cycle->head + sz)%cycle->cap;
	}else{
		cycle->head = cycle->tail = 0;
	}
}
int
wod_cycle_buffer_get_used(wod_cycle_buffer_t* cycle,wod_cycle_pair_t * pair)
{
	if(cycle->head == cycle->tail){
		return -1;
	}
	if(pair){
		if(cycle->head > cycle->tail){
			pair->first.buf = cycle->buf+cycle->head;
			pair->first.sz = cycle->cap - cycle->head;
			pair->second.buf = cycle->buf;
			pair->second.sz = cycle->tail;
		}else{
			pair->first.buf = cycle->buf+cycle->head;
			pair->first.sz = cycle->tail - cycle->head;
			pair->second.buf = NULL;
			pair->second.sz = 0;
		}
	}
	return 0;
}
int
wod_cycle_buffer_grow(wod_cycle_buffer_t* cycle,int freesize,wod_cycle_pair_t *pair)
{
	if( freesize<0){
		return -1;
	}
	if( freesize == 0){
		freesize = _get_free_sz(cycle);
		if(freesize == 0){
			return -1;
		}
	}
	else if( _resize_byuse(cycle,freesize) != 0){
		return -1;
	}
	if(cycle->head > cycle->tail){
		pair->first.buf = cycle->buf + cycle->tail;
		pair->first.sz = freesize;
		pair->second.buf = NULL;
		pair->second.sz = 0;
		cycle->tail+=freesize;
	}else{
		int efz = cycle->cap - cycle->tail;
		if(freesize > efz){
			pair->first.buf = cycle->buf + cycle->tail;
			pair->first.sz = efz;
			pair->second.buf = cycle->buf;
			pair->second.sz = cycle->head;
			cycle->tail = freesize-efz;
		}else{
			pair->first.buf = cycle->buf + cycle->tail;
			pair->first.sz = freesize;
			pair->second.buf = NULL;
			pair->second.sz = 0;
			cycle->tail+=freesize;
		}
	}
	return 0;
}
void
wod_cycle_buffer_back(wod_cycle_buffer_t* cycle,int backsz)
{
	int usedsz = cycle->cap - _get_free_sz(cycle);
	if(usedsz > backsz){
		cycle->tail = (cycle->tail - backsz + cycle->cap)%cycle->cap;
	}else{
		cycle->head = cycle->tail = 0;
	}
}
void
wod_cycle_buffer_destroy( wod_cycle_buffer_t* cycle )
{
	free(cycle->buf);
}
static int
_get_free_sz(wod_cycle_buffer_t* cycle)
{//可用的SIZE
	int freesize = 0;
	if(cycle->head > cycle->tail){
		freesize = cycle->head - cycle->tail;
	}else if(cycle->head < cycle->tail){
		freesize = cycle->cap - cycle->tail + cycle->head ;
	}else{
		freesize = cycle->cap-1;
	}
	return freesize;
}
bool
wod_cycle_buffer_empty( wod_cycle_buffer_t* cycle )
{
	return (cycle->head == cycle->tail);
}
static int
_resize_byuse(wod_cycle_buffer_t* cycle,int usesz)
{
	int freesize = _get_free_sz(cycle);
	int newodaps = cycle->cap;
	int tmp = freesize;
	while(tmp <= usesz){
		tmp += newodaps;
		newodaps*=2;
	}
	if(tmp > freesize){
		void * newbuf;
		if(cycle->head == cycle->tail){
			newbuf = realloc(cycle->buf,newodaps);
			if(!newbuf){
				return -1;
			}
			cycle->buf = newbuf;
			cycle->head = 0;
			cycle->tail = 0;
			cycle->cap = newodaps;
		}else{
			newbuf = malloc(newodaps);
			if(!newbuf){
				return -1;
			}
			if(cycle->head > cycle->tail){
				int fsz = cycle->cap-cycle->head;
				int esz = cycle->tail;
				memcpy(newbuf,cycle->buf+cycle->head,fsz);
				memcpy(newbuf,cycle->buf,esz);
				cycle->head = 0;
				cycle->tail = fsz+esz;
				cycle->cap = newodaps;
			}else{
				int sz = cycle->tail-cycle->head;
				memcpy(newbuf,cycle->buf+cycle->head,sz);

				cycle->head = 0;
				cycle->tail = sz;
				cycle->cap = newodaps;
			}
			free(cycle->buf);
			cycle->buf = newbuf;
		}
	}
	return 0;
}