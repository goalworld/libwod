/*
 * cyclebuf_test.c
 *
 *  Created on: 2013-3-8
 *      Author: wd
 */


#include "wod_cyclebuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define GROW_SZ 939
int main(int argc, char const *argv[])
{
	wod_cycle_buffer_t buf;
	wod_cycle_buffer_init(&buf,4096);
	int i;
	size_t sz;
	for(i=0;i<100;i++){
		wod_cycle_buffer_grow(&buf,GROW_SZ);
		sz = wod_cycle_buffer_used_size(&buf);
		assert(sz == (i+1)*GROW_SZ);
	}
	for(;i>0;i--){
		wod_cycle_buffer_pop(&buf,GROW_SZ);
		sz = wod_cycle_buffer_used_size(&buf);
		assert(sz == (i-1)*GROW_SZ);
	}
	return 0;
}
