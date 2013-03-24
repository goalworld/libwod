#include "wod_time.h"
#include <sys/time.h>
#include <stddef.h>
#include <unistd.h>
wod_i64_t
wod_time_usecond()
{
	struct timeval time;
	gettimeofday(&time,NULL);
	return time.tv_sec*1000000 + time.tv_usec;
}
void
wod_usleep(wod_i64_t usec)
{
	struct timeval time;
	time.tv_sec = usec/1000000;
	time.tv_usec = usec%1000000;
	select(0,NULL,NULL,NULL,&time);
}
