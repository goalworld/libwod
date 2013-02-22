#include "wod_array.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	struct wodArray arr;
	wodArrayInit(&arr,sizeof(int));
	int i ,j;
	for( i=0; i< 10000; i++ ){
		if(i %2 ){
			wodArrayPush(&arr,&i);
		}else{
			wodArrayUnShift(&arr,&i);
		}
		printf(" size %d\n", wodArraySize(&arr));
	}
	for( i=0;i< 10000;i++){
		wodArrayAt(&arr,i,&j);
		printf("[%d:%d]\n", i,j);
	}
	return 0;
}
