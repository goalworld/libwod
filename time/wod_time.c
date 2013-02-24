#include "wod_time.h"
#include <unistd.h>
#include <sys/time.h>
long long
wod_time_usecond()
{
	struct timeval time;
	gettimeofday(&time,NULL);
	return time.tv_sec*1000000 + time.tv_usec;
}

void
wod_time_sleep_usecond(long long usec)
{
	if( usec <= 0 ){
		return ;
	}
	struct timeval tv;
	tv.tv_sec = usec/1000000;
	tv.tv_usec = usec - tv.tv_sec*1000000;
	select(0,NULL,NULL,NULL,&tv);
}