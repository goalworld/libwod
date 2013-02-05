#include "array.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	wcArray arr;
	wcArrayInit(&arr,sizeof(int));
	int i ,j;
	for( i=0; i< 10000; i++ ){
		if(i %2 ){
			wcArrayPush(&arr,&i);
		}else{
			wcArrayUnShift(&arr,&i);
		}
		
	}
	for( i=0;i< 10000;i++){
		wcArrayAt(&arr,i,&j);
		printf("%d\n", j);
	}
	return 0;
}