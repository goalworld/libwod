#include "wod_net.h"
#include "wod_ev.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SZ 1024
static void _doAccept(struct wodEvLoop *loop,void * nv,int mask);
static void _doRead(struct wodEvLoop *loop,void * nv,int mask);

int 
main(int argc, char const *argv[])
{
	struct wodEvLoop *loop;
	loop = wodEvLoopNew(10240,WV_POLL_POLL);
	if( !loop ){
		printf("%s\n", "loop create error" );
		return 1;
	}
	wodNetFd fd = wodNetTcpListen(TCP4,"0.0.0.0",8100);
	if(fd < 0){
		printf("%s\n", strerror(-fd) );
		return 1;
	}
	printf("%d\n",fd);
	wodNetSetNonBlock(fd,1);
	wodEvIOAdd(loop,fd,WV_IO_READ,_doAccept,(void *)(intptr_t)(fd));
	wodEvRun(loop);
	return 0;
}
static void 
_doAccept(struct wodEvLoop *loop,void * nv,int mask)
{
	wodNetFd fd = (wodNetFd)(long)(nv);
	wodNetFd cfd = wodNetAccept(fd);
	
	if(cfd > 0){
		printf("Connected : fd%d\n",cfd );
		wodNetSetNonBlock(cfd,1);
		wodEvIOAdd(loop,cfd,WV_IO_READ,_doRead,(void *)(intptr_t)(cfd));
		
	}else if(cfd < 0 && -cfd == EAGAIN){
		if(-cfd == EAGAIN){
			perror("wodNetAccept00000000000");
		}else{
			perror("wodNetAccept");
		}
	}
}
static void 
_doRead(struct wodEvLoop *loop,void * nv,int mask)
{
	wodNetFd fd = (wodNetFd)(long)(nv);
	unsigned char buf[BUF_SZ+1];
	int nr = wodNetRead(fd,buf,BUF_SZ);
	if( nr <= 0){
		perror("read");
		wodEvIORemove(loop,fd,WV_IO_READ);
		return;
	}
	buf[nr] = 0;
	printf("read===> %s %d\n", buf,nr);
}
