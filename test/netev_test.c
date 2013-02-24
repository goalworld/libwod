#include "wod_net.h"
#include "wod_event.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SZ 1024
static void _doAccept(struct wod_event_main *loop,void * nv,int mask);
static void _doRead(struct wod_event_main *loop,void * nv,int mask);

int 
main(int argc, char const *argv[])
{
	struct wod_event_main *loop;
	loop = wod_event_main_new(10240,WV_POLL_POLL);
	if( !loop ){
		printf("%s\n", "loop create error" );
		return 1;
	}
	wod_socket_t fd = wod_net_tcp_listen(TCP4,"0.0.0.0",8100);
	if(fd < 0){
		printf("%s\n", strerror(-fd) );
		return 1;
	}
	printf("%d\n",fd);
	wod_net_noblock(fd,1);
	wod_event_io_add(loop,fd,WV_IO_READ,_doAccept,(void *)(intptr_t)(fd));
	wod_event_main_loop(loop);
	return 0;
}
static void 
_doAccept(struct wod_event_main *loop,void * nv,int mask)
{
	wod_socket_t fd = (wod_socket_t)(long)(nv);
	wod_socket_t cfd = wod_net_accept(fd);
	
	if(cfd > 0){
		printf("Connected : fd%d\n",cfd );
		wod_net_noblock(cfd,1);
		wod_event_io_add(loop,cfd,WV_IO_READ,_doRead,(void *)(intptr_t)(cfd));
		
	}else if(cfd < 0 && -cfd == EAGAIN){
		if(-cfd == EAGAIN){
			perror("wodNetAccept00000000000");
		}else{
			perror("wodNetAccept");
		}
	}
}
static void 
_doRead(struct wod_event_main *loop,void * nv,int mask)
{
	wod_socket_t fd = (wod_socket_t)(long)(nv);
	unsigned char buf[BUF_SZ+1];
	int nr = wod_net_read(fd,buf,BUF_SZ);
	if( nr <= 0){
		perror("read");
		wod_event_io_remove(loop,fd,WV_IO_READ);
		return;
	}
	buf[nr] = 0;
	printf("read===> %s %d\n", buf,nr);
}
