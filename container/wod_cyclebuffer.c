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
static int _resize_byuse(wod_cycle_buffer_t* cycle,size_t usesz);
static int 	_get_free_sz(wod_cycle_buffer_t* cycle);
int
wod_cycle_buffer_init( wod_cycle_buffer_t* cycle,size_t defsize)
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
wod_cycle_buffer_push(wod_cycle_buffer_t* cycle,void *buf,size_t bufsz)
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
wod_cycle_buffer_pop(wod_cycle_buffer_t* cycle,size_t sz)
{
	int usedsz = cycle->cap - _get_free_sz(cycle);
	if(usedsz > sz){
		cycle->head = (cycle->head + sz)%cycle->cap;
	}else{
		cycle->head = cycle->tail = 0;
	}
}
size_t
wod_cycle_buffer_used_size(wod_cycle_buffer_t* cycle)
{
	if(cycle->head == cycle->tail){
		return 0;
	}
	if(cycle->head > cycle->tail){
		return  cycle->cap - cycle->head+cycle->tail;
	}else{
		return cycle->tail - cycle->head;
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
wod_cycle_buffer_get_unused(wod_cycle_buffer_t* cycle,wod_cycle_pair_t * pair,size_t needsz)
{
	if( needsz == 0){
		needsz = _get_free_sz(cycle);
		if(needsz == 0){
			return -1;
		}
	}
	else if( _resize_byuse(cycle,needsz) != 0){
		return -1;
	}
	if(cycle->head > cycle->tail){
		pair->first.buf = cycle->buf + cycle->tail;
		pair->first.sz = needsz;
		pair->second.buf = NULL;
		pair->second.sz = 0;
	}else{
		int efz = cycle->cap - cycle->tail;
		if(needsz > efz){
			pair->first.buf = cycle->buf + cycle->tail;
			pair->first.sz = efz;
			pair->second.buf = cycle->buf;
			pair->second.sz = cycle->head;
		}else{
			pair->first.buf = cycle->buf + cycle->tail;
			pair->first.sz = needsz;
			pair->second.buf = NULL;
			pair->second.sz = 0;
		}
	}
	return 0;
}
int
wod_cycle_buffer_grow(wod_cycle_buffer_t* cycle,size_t freesize)
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
		cycle->tail+=freesize;
	}else{
		int efz = cycle->cap - cycle->tail;
		if(freesize > efz){
			cycle->tail = freesize-efz;
		}else{
			cycle->tail+=freesize;
		}
	}
	return 0;
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
int
wod_cycle_pair_readsz(wod_cycle_pair_t * pair,void *buf,size_t sz)
{
	int all = wod_cycle_pair_sz(pair);
	if(all < sz){
		return -1;
	}
	if(pair->first.sz > sz){
		memcpy(buf,pair->first.buf,sz);
		pair->first.sz -=  sz;
		pair->first.buf = (char *)pair->first.buf + sz;
		return 0;
	}
	if(pair->first.sz > 0){
		memcpy(buf,pair->first.buf,pair->first.sz);
		buf = (char *)buf+pair->first.sz;
		sz -= pair->first.sz;
		pair->first.sz = 0;
		pair->first.buf = NULL;
	}
	memcpy(buf,pair->second.buf,sz);
	pair->second.sz -=  sz;
	pair->second.buf = (char *)pair->second.buf + sz;
	return 0;
}
static int
_resize_byuse(wod_cycle_buffer_t* cycle,size_t usesz)
{
	size_t freesize = _get_free_sz(cycle);
	size_t newodaps = cycle->cap;
	size_t tmp = freesize;
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
				size_t fsz = cycle->cap-cycle->head;
				size_t esz = cycle->tail;
				memcpy(newbuf,cycle->buf+cycle->head,fsz);
				memcpy(newbuf,cycle->buf,esz);
				cycle->head = 0;
				cycle->tail = fsz+esz;
				cycle->cap = newodaps;
			}else{
				size_t sz = cycle->tail-cycle->head;
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
