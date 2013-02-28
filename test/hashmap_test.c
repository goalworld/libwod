/*
 * hashmap_test.c
 *
 *  Created on: 2013-2-8
 *      Author: Administrator
 */


#include <stdio.h>
#include <stdlib.h>
#include "wod_hashmap.h"
#include <time.h>
#include <memory.h>
 #include <assert.h>
#include <wod_time.h>
static unsigned 	
hash_func(void *env,const void *key)
{
	return (unsigned)key;
}
static void test(unsigned  num );
int main(int argc, char const *argv[])
{
	test(1000);
	test(10000);
	test(100000);
	test(1000000);
	test(10000000);
	//test(100000000);
	return 0;
}
static void 
test(unsigned  num )
{
	struct wod_hash_map_type whmt;
	memset(&whmt,0,sizeof(whmt));
	whmt.hash_func = hash_func;
	struct wod_hash_map *hm = wod_hashmap_new(whmt,NULL);
	int i=0;
	long long pre = wod_time_usecond(),df;
	for(;i<num;i++){
		wod_hashmap_insert(hm,(void *)i,(void *)i);
	}
	printf("[BEGIN]number : %d\n",num);
	df = wod_time_usecond()-pre;
	printf("[Insert]%d >>> all:%f---one:%f\n",num,df,df/num);
	pre = wod_time_usecond();
	for( i=0;i<num;i++){
		wod_hashmap_query(hm,(void *)i);
	}
	df = wod_time_usecond()-pre;
	printf("[Query] %d>>> all:%ld---one:%ld\n",num,df,df/num);
	puts("[END]");
	wod_hashmap_delete(hm);
}
