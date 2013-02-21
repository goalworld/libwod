/*
 * rain_array.c
 *
 *  Created on: 2012-11-14
 *      Author: goalworld
 */


#include "wod_array.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
void
wcArrayInit(struct wcArray *arr,int elemsz)
{
	assert(arr);
	arr->elem_sz = elemsz;
	arr->arr_data = malloc(elemsz * 64);
	arr->arr_sz = 64;
	arr->cut_sz = 0;
}
static inline void
_add_size(struct wcArray *arr,int elem)
{
	if(elem <= arr->arr_sz){
		return;
	}
	int sz = arr->arr_sz*2;
	for(;;){
		if(sz >= elem ){
			arr->arr_data = realloc(arr->arr_data,arr->elem_sz * sz);
			arr->arr_sz = sz;
			break;
		}
		sz*=2;
	}
}
void
wcArrayDestroy(struct wcArray *arr)
{
	assert(arr);
	free(arr->arr_data);
}
void
wcArrayPush(struct wcArray *arr,void *elem)
{
	assert(arr);
	_add_size(arr,arr->cut_sz+1);
	memcpy((char*)(arr->arr_data)+arr->cut_sz*arr->elem_sz,
			elem,
			arr->elem_sz);
	arr->cut_sz++;
}
void
wcArrayAt(struct wcArray *arr,int index,void *elem)
{
	assert(arr && index<arr->cut_sz);
	memcpy(elem,
			(char*)(arr->arr_data)+arr->elem_sz*index,
			arr->elem_sz);
}
void
wcArraySet(struct wcArray *arr,int index,void *elem)
{
	assert(arr && index<=arr->cut_sz);
	memcpy((char*)(arr->arr_data)+arr->elem_sz*index,
			elem,
			arr->elem_sz);
}
void
wcArrayUnShift(struct wcArray *arr,void *elem)
{
	assert(arr);
	_add_size(arr,arr->cut_sz+1);
	memmove((char*)(arr->arr_data)+1*arr->elem_sz,
			(char*)(arr->arr_data),
			arr->cut_sz*arr->elem_sz);
	memcpy((char*)(arr->arr_data),
			elem,
			arr->elem_sz);
	arr->cut_sz++;
}
void
wcArrayErase(struct wcArray *arr,int index,int numelem,void *elem)
{
	assert(arr && index<=arr->cut_sz && numelem <= arr->cut_sz-index);
	if(elem){
		memcpy(elem,
				(char*)(arr->arr_data) + (index)*arr->elem_sz,
				numelem*arr->elem_sz);
	}
	if((arr->cut_sz-index-numelem)*arr->elem_sz){
		memmove((char*)(arr->arr_data) + index*arr->elem_sz,
			(char*)(arr->arr_data) + (index+numelem)*arr->elem_sz,
			(arr->cut_sz-index-numelem)*arr->elem_sz);
	}
	arr->cut_sz-=numelem;
}
void
wcArrayInsert(struct wcArray *arr,int index,void *elem,int numelem)
{
	assert(arr && index<=arr->cut_sz);
	_add_size(arr,arr->cut_sz+numelem);
	memmove((char*)(arr->arr_data) + (index+numelem)*arr->elem_sz,
			(char*)(arr->arr_data) + (index)*arr->elem_sz,
			(arr->cut_sz-index)*arr->elem_sz);
	memcpy((char*)(arr->arr_data) + (index)*arr->elem_sz,
			elem,
			numelem*arr->elem_sz);
	arr->cut_sz+=numelem;
}
unsigned
wcArraySize(struct wcArray *arr)
{
	assert(arr);
	return arr->cut_sz;
}
