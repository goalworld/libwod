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
_get_addr(enum wodNetTCP et,const char *ip,short port,SA **outaddr,size_t *len,int *fam)
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
wod_net_tcp_listen(enum wodNetTCP et,const char * ip,int port)
{
	SA *addr;
	size_t len;
	int family;
	int ret;
	if( (ret = _get_addr(et,ip,port,&addr,&len,&family)) < 0){
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
	int flag = 1;
	if( setsockopt(sfd,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag)) < 0){
		return -errno;
	}
	return sfd;
}
wod_socket_t
wod_net_tcp_connect(enum wodNetTCP et,const char * ip,int port)
{
	SA *addr;
	size_t len;
	int family;
	int ret;
	if( (ret = _get_addr(et,ip,port,&addr,&len,&family)) < 0){
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
wod_net_accept( wod_socket_t sfd)
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
wod_net_writev(wod_socket_t fd,struct wod_socket_buf *bufs,size_t bufslen)
{
	int nw;
	int allwrite = 0;
	int alllen = 0;
	int i=0;
	for(i=0;i<bufslen;i++){
		alllen+=bufs[i].b_sz;
	}
	for(;;){
		if((nw = writev(fd,(struct iovec*)bufs,bufslen)) < 0){
			if(errno == EINTR) continue;
			else if(allwrite > 0){
				return allwrite;
			}else{
				return nw;
			}
		}else if(nw == 0){
			return allwrite;
		}
		allwrite+=nw;
		if(allwrite == alllen){
			return allwrite;
		}
	}
	return allwrite;
}
int  
wod_net_write(wod_socket_t fd,void *buf,size_t sz)
{
	int nw;
	int allwrite = 0;
	for(;;){
		if((nw = write(fd,buf,sz)) < 0){
			if(errno == EINTR) continue;
			else if(allwrite > 0){
				return allwrite;
			}else{
				return nw;
			}
		}else if(nw == 0){
			return allwrite;
		}
		allwrite+=nw;
		if(allwrite == sz){
			return allwrite;
		}
	}
	return allwrite;
}
int  
wod_net_readv(wod_socket_t fd,struct wod_socket_buf *bufs,size_t bufslen)
{
	int nr;
	int allread = 0;
	int alllen = 0;
	int i=0;
	for(i=0;i<bufslen;i++){
		alllen+=bufs[i].b_sz;
	}
	for(;;){
		if((nr = readv(fd,(struct iovec*)bufs,bufslen)) <= 0){
			if(nr < 0 && errno == EINTR) continue;
			else if(allread > 0){
				return allread;
			}else{
				return nr;
			}
		}
		allread+=nr;
		if(alllen == allread){
			return allread;
		}
	}
	return allread;
}
int  
wod_net_read_full(wod_socket_t fd,void *buf,size_t sz)
{
	return 0;
}
int  
wod_net_read(wod_socket_t fd,void *buf,size_t sz)
{
	int nr;
	int allread = 0;
	for(;;){
		if((nr = read(fd,buf,sz)) <= 0){
			if(nr < 0 && errno == EINTR)continue;
			else if(allread >0){
				return allread;
			}else{
				return nr;
			}
		}
		allread+=nr;
		if(allread == sz){
			return allread;
		}
	}
	return nr;
}

int  
wod_net_close(wod_socket_t fd)
{
	int ret = close(fd);
	if(ret < 0){
		return -errno;
	}
	return ret;
}
int  
wod_net_noblock(wod_socket_t fd,int flag)
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
wod_net_keep_alive(wod_socket_t fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));
}
int  
wod_net_nodelay(wod_socket_t fd,int flag)
{
	return setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag));
}
int  
wod_net_recv_buffer_size(wod_socket_t fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&flag,sizeof(flag));
}
int  
wod_net_send_buffer_size(wod_socket_t fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_SNDBUF,&flag,sizeof(flag));
} 	
