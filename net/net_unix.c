#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "wod_net.h"
#include <stddef.h>
/*
#include <arpa/inet.h>

p(presentation)n(numeric)
int inet_pton(
	int family,//AF_INET AF_INET6
	const char * srptr,
	void * addrptr//sockaddr_in.sin_addr sockaddr_in6.sin6_addr
) 
0:arg error 1:suc -1:error
const char * inet_ntop(
	int fm,//AF_INET AF_INET6
	const void *addrptr,//sockaddr_in.sin_addr sockaddr_in6.sin6_addr
	char *strptr,
	size_t len//INET_ADDRSTRLEN(16) INET6_ADDRSTRLEN(46)
);//NULL error 
*/
#define SA struct sockaddr 
static int 
_getAddr(enum wodNetTCP et,const char *ip,short port,SA **outaddr,size_t *len,int *fam)
{
	int family;
	if(et == TCP4){
		family = AF_INET;
		*len = sizeof(struct sockaddr_in);
		struct sockaddr_in *addr = malloc(*len);
		bzero(addr,*len);
		addr->sin_family = family;
		addr->sin_port = htons(port);
		if( inet_pton(family,ip,&addr->sin_addr) != 1){
			free(addr);
			return -errno;
		}
		*outaddr = (SA*)addr;
	}else if(et == TCP6){
		family = AF_INET6;
		*len = sizeof(struct sockaddr_in6);
		struct sockaddr_in6 *addr = malloc(*len);
		bzero(&addr,*len);
		addr->sin6_family = family;
		addr->sin6_port = htons(port);
		if( inet_pton(family,ip,&addr->sin6_addr) != 1){
			free(addr);
			return -errno;
		}
		*outaddr = (SA*)addr;
	}
	else{
		return -EINVAL;
	}
	*fam = family;
	return 0;
}
wod_socket_t
wod_tcp_listen(enum wodNetTCP et,const char * ip,int port)
{
	SA *addr;
	size_t len;
	int family;
	int ret;
	if( (ret = _getAddr(et,ip,port,&addr,&len,&family)) < 0){
		return ret;
	}
	wod_socket_t sfd = socket(family,SOCK_STREAM,0);
	if(sfd < 0){
		return -errno;
	}
	if(bind(sfd,addr,len) < 0){
		free(addr);
		return -errno;
	}
	free(addr);
	if(listen(sfd,64) < 0){
		return -errno;
	}
	return sfd;
}
wod_socket_t
wod_tcp_connect(enum wodNetTCP et,const char * ip,int port)
{
	SA *addr;
	size_t len;
	int family;
	int ret;
	if( (ret = _getAddr(et,ip,port,&addr,&len,&family)) < 0){
		return ret;
	}
	wod_socket_t sfd = socket(family,SOCK_STREAM,0);
	if(sfd < 0){
		return -errno;
	}
	if(connect(sfd,addr,len) < 0){
		free(addr);
		return -errno;
	}
	free(addr);
	return sfd;
}
wod_socket_t
wod_accept( wod_socket_t sfd)
{
	int ret;
	for(;;){
		if( (ret = accept(sfd,NULL,NULL)) < 0){
			if(errno == EINTR){
				continue;
			}
			ret = -errno;
		}
		break;
	}
	return ret;
}

int  
wod_remote_addr(wod_socket_t fd,char * strptr,size_t len,int *port)
{
	return 0;
}
int  
wod_locate_addr(wod_socket_t fd,char * strptr,size_t len,int *port)
{
	return 0;
}
int  
wod_writev(wod_socket_t fd,struct wod_socket_buf *bufs,size_t bufslen)
{
	int nw;
	for(;;){
		if((nw = writev(fd,(struct iovec*)bufs,bufslen)) < 0){
			if(errno == EINTR) continue;
			nw = -errno;
		}
		break;
	}
	return nw;
}
int  
wod_write(wod_socket_t fd,void *buf,size_t sz)
{
	int nw;
	for(;;){
		if((nw = write(fd,buf,sz)) < 0){
			if(errno == EINTR) continue;
			nw = -errno;
		}
		break;
	}
	return nw;
}
int  
wod_readv(wod_socket_t fd,struct wod_socket_buf *bufs,size_t bufslen)
{
	int nr;
	for(;;){
		if((nr = readv(fd,(struct iovec*)bufs,bufslen)) < 0){
			if(errno == EINTR) continue;
			nr = -errno;
		}
		break;
	}
	return nr;
}
int  
wod_read_full(wod_socket_t fd,void *buf,size_t sz)
{
	return 0;
}
int  
wod_read(wod_socket_t fd,void *buf,size_t sz)
{
	int nr;
	for(;;){
		if((nr = read(fd,buf,sz)) < 0){
			if(errno == EINTR)continue;
			nr = -errno;
		}
		break;
	}
	return nr;
}

int  
wod_close(wod_socket_t fd)
{
	int ret = close(fd);
	if(ret < 0){
		return -errno;
	}
	return ret;
}
int  
wod_set_noblock(wod_socket_t fd,int flag)
{
	int oflag = fcntl(fd, F_GETFL, 0);
	if(flag){
		oflag |= O_NONBLOCK;
	}else{
		oflag &= (~O_NONBLOCK);
	}
    
    if( fcntl(fd, F_SETFL, oflag) < 0){
    	return -errno;
    }
    return 0;
}
int  
wod_set_keep_alive(wod_socket_t fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));
}
int  
wod_set_nodelay(wod_socket_t fd,int flag)
{
	return setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag));
}
int  
wod_set_recv_buffer_size(wod_socket_t fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&flag,sizeof(flag));
}
int  
wod_set_send_buffer_size(wod_socket_t fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_SNDBUF,&flag,sizeof(flag));
} 	
