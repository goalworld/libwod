/*
Date:2013.2.2
Auther:goalworld

*/
#ifndef _WOD_NET_H_
#define _WOD_NET_H_
#include <stdint.h>
#include <stddef.h>

#if HAS_OS_WINDOWS
#define <windows.h>
typedef SOCKET wod_socket_t;
#else
typedef int wod_socket_t;
#endif

enum wodNetTCP{
	TCP4,
	TCP6
};
struct wod_socket_buf{
	void * b_body;
	size_t b_sz;
};
wod_socket_t wod_net_tcp_listen(enum wodNetTCP,const char * addr,int port);
wod_socket_t wod_net_tcp_connect(enum wodNetTCP,const char * addr,int port);
wod_socket_t wod_net_accept( wod_socket_t fd);
int  wod_net_close(wod_socket_t fd);

int  wod_remote_addr(wod_socket_t fd,char * strptr,size_t len,int *port);
int  wod_locate_addr(wod_socket_t fd,char * strptr,size_t len,int *port);
int  wod_net_writev(wod_socket_t fd ,struct wod_socket_buf *bufs,size_t bufslen);
int  wod_net_write(wod_socket_t fd,void *buf,size_t sz);
int  wod_net_readv(wod_socket_t fd,struct wod_socket_buf *bufs,size_t bufslen);
int  wod_net_read(wod_socket_t fd,void *buf,size_t sz);

int  wod_net_read_full(wod_socket_t fd,void *buf,size_t sz);


int  wod_net_keep_alive(wod_socket_t fd,int flag);//0,1
int  wod_net_nodelay(wod_socket_t fd,int flag);//0,1
int  wod_net_noblock(wod_socket_t fd,int flag);//0,1
int  wod_net_recv_buffer_size(wod_socket_t fd,int flag);//SIZE
int  wod_net_send_buffer_size(wod_socket_t fd,int flag);//SIZE

#endif
