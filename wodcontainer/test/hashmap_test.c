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
unsigned 	hashFunc(void *env,const void *key)
{
	return (unsigned)key;
}
int 		keyCampre(void *env,void *key1,void *key2)
{
	return 0;
}
void *		keyClone(void *env,void *key)
{
	return NULL;
}
void *		valueClone(void *env,void *value)
{
	return NULL;
}
void 		keyDestroy(void *env,void *key)
{

}
void 		valueDestroy(void *env,void *value)
{

}
int main(int argc, char const *argv[])
{
	struct wcHashMapType whmt;
	whmt.hashFunc = hashFunc;
	struct wcHashMap *hm = wcHashMapNew(whmt,NULL);
	int i=0;
	clock_t pre = clock(),df;
	for(;i<10000000;i++){
		wcHashMapInsert(hm,(void *)i,(void *)i);
	}
	df = clock()-pre;
	printf("wcHashMapInsert%d---%f\n",df,(double)df*1E-7);
	pre = clock();
	for( i=0;i<10000000;i++){
		wcHashMapRemove(hm,(void *)i);
	}
	df = clock()-pre;
	printf("wcHashMapQuery%d---%f\n",df,(double)df*1E-7);
	wcHashMapDelete(hm);
	return 0;
}
