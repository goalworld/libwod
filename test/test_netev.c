#include <wod/net.h>
#include <wod/ev.h>
#include <string.h>
#include <stdio.h>
#define BUF_SZ 1024
static void _doAccept(void * nv,int mask);
static void _doRead(void * nv,int mask);
struct wvLoop *loop;
int 
main(int argc, char const *argv[])
{
	loop = wvLoopNew(10240,WV_POLL_SELECT);
	if( !loop ){
		printf("%s\n", "loop create error" );
		return 1;
	}
	wnFd fd = wnTcpListen(TCP4,"0.0.0.0",8100);
	if(fd < 0){
		printf("%s\n", strerror(-fd) );
		return 1;
	}
	printf("%d\n",fd);
	wnSetNonBlock(fd,1);
	wvIOAdd(loop,fd,WV_IO_READ,_doAccept,(void *)(fd));
	wvRun(loop);
	return 0;
}
static void 
_doAccept(void * nv,int mask)
{
	wnFd fd = (wnFd)(long)(nv);
	wnFd cfd = wnAccept(fd);
	printf("%d\n",fd);
	if(cfd > 0)
		wvIOAdd(loop,cfd,WV_IO_READ,_doRead,(void *)(cfd));
}
static void 
_doRead(void * nv,int mask)
{
	wnFd fd = (wnFd)(long)(nv);
	unsigned char buf[BUF_SZ+1];
	int nr = wnRead(fd,buf,BUF_SZ);
	if( nr <= 0){
		perror("read");
		wvIORemove(loop,fd,WV_IO_READ);
		return;
	}
	buf[nr] = 0;
	printf("%s\n", buf+4);
}
