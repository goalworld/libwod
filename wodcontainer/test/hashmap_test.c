/*
 * hashmap_test.c
 *
 *  Created on: 2013-2-8
 *      Author: Administrator
 */


#include <stdio.h>
#include <stdlib.h>
#include "../include/hashmap.h"
#include <time.h>
#include <memory.h>
unsigned 	hashFunc(void *env,const void *key)
{
	return (unsigned)key;
}
int main(int argc, char const *argv[])
{
	struct wcHashMapType whmt;
	memset(&whmt,0,sizeof(whmt));
	whmt.hashFunc = hashFunc;
	struct wcHashMap *hm = wcHashMapNew(whmt,NULL);
	int i=0;
	clock_t pre = clock(),df;
	for(;i<10000000;i++){
		wcHashMapInsert(hm,(void *)i,(void *)i);
	}
	df = clock()-pre;
	printf("wcHashMapInsert%ld---%f\n",df,(double)df*1E-7);
	pre = clock();
	for( i=0;i<10000000;i++){
		wcHashMapQuery(hm,(void *)i);
	}
	df = clock()-pre;
	printf("wcHashMapQuery%ld---%f\n",df,(double)df*1E-7);
	wcHashMapDelete(hm);
	return 0;
}
