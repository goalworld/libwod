#include "wod_net.h"
#include "wod_event.h"
#include "wod_errno.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#define BUF_SZ 1024
static void _doAccept(struct wod_event *loop,void * nv,int mask);
static void _doRead(struct wod_event *loop,void * nv,int mask);
static int _doTimer(struct wod_event * loop,void * nv);
int 
main(int argc, char const *argv[])
{
	struct wod_event *loop;
	if( wod_event_create(&loop,10240,WV_POLL_EPOLL) != WOD_OK){
		printf("%s\n", "loop create error" );
		return 1;
	}
	wod_socket_t fd = wod_net_tcp_listen(TCP4,"0.0.0.0",8194);
	if(fd < 0){
		printf("%s\n", strerror(-fd) );
		return 1;
	}
	printf("%d\n",fd);
	wod_net_noblock(fd,1);
	wod_event_io_add(loop,fd,WV_IO_READ,_doAccept,(void *)(intptr_t)(fd));
	//int id =
	//wod_event_time_add(loop,10000,_doTimer,NULL);
	for(;;){
		wod_event_once(loop);
	}

	return 0;
}
static int
_doTimer(struct wod_event * loop,void * nv)
{
	static wod_i64_t pre = 0;
	wod_i64_t cut = wod_time_usecond();
	printf("hello  %lld\n",cut-pre);
	pre = cut;
	return WOD_OK;
}
static void
_doAccept(struct wod_event *loop,void * nv,int mask)
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
_doRead(struct wod_event *loop,void * nv,int mask)
{
	wod_socket_t fd = (wod_socket_t)(long)(nv);
	unsigned char buf[BUF_SZ+1];
	int nr = wod_net_read(fd,buf,BUF_SZ);
	if( nr <= 0){
		perror("read");
		wod_event_io_remove(loop,fd,WV_IO_READ);
		wod_net_close(fd);
		return;
	}
	buf[nr] = 0;
	printf("read===> %s %d\n", buf,nr);
}
