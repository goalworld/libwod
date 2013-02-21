#include "wod_array.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	struct wod_array arr;
	wod_array_init(&arr,sizeof(int));
	int i ,j;
	for( i=0; i< 10000; i++ ){
		if(i %2 ){
			wod_array_push(&arr,&i);
		}else{
			wod_array_unshift(&arr,&i);
		}
		printf(" size %d\n", wod_array_size(&arr));
	}
	for( i=0;i< 10000;i++){
		wod_array_at(&arr,i,&j);
		printf("[%d:%d]\n", i,j);
	}
	return 0;
}
