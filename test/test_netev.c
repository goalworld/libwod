#include <wod/net.h>
#include <wod/ev.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SZ 1024
static void _doAccept(struct wvLoop *loop,void * nv,int mask);
static void _doRead(struct wvLoop *loop,void * nv,int mask);

int 
main(int argc, char const *argv[])
{
	struct wvLoop *loop;
	loop = wvLoopNew(10240,WV_POLL_POLL);
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
_doAccept(struct wvLoop *loop,void * nv,int mask)
{
	wnFd fd = (wnFd)(long)(nv);
	wnFd cfd = wnAccept(fd);
	
	if(cfd > 0){
		printf("Connected : fd%d\n",cfd );
		wvIOAdd(loop,cfd,WV_IO_READ,_doRead,(void *)(cfd));
	}else if(cfd < 0 && -cfd == EAGAIN){
		if(-cfd == EAGAIN){
			perror("wnAccept00000000000");
		}else{
			perror("wnAccept");
		}
	}
}
static void 
_doRead(struct wvLoop *loop,void * nv,int mask)
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
