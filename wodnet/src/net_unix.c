#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "net.h"
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
_getAddr(enum wnTCP et,const char *ip,short port,SA **outaddr,size_t *len,int *fam)
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
wnFd 
wnTcpListen(enum wnTCP et,const char * ip,int port)
{
	SA *addr;
	size_t len;
	int family;
	int ret;
	if( (ret = _getAddr(et,ip,port,&addr,&len,&family)) < 0){
		return ret;
	}
	wnFd sfd = socket(family,SOCK_STREAM,0);
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
wnFd 
wnTcpConnect(enum wnTCP et,const char * ip,int port)
{
	SA *addr;
	size_t len;
	int family;
	int ret;
	if( (ret = _getAddr(et,ip,port,&addr,&len,&family)) < 0){
		return ret;
	}
	wnFd sfd = socket(family,SOCK_STREAM,0);
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
wnFd 
wnAccept( wnFd sfd)
{
	int ret;
	for(;;){
		if( (ret = accept(sfd,NULL,NULL)) < 0){
			if(ret < 0){
				if(errno == EINTR){
					continue;
				}
				ret = -errno;
			}
		}
		break;
	}
	return ret;
}

int  
wnRemoteAddr(wnFd fd,char * strptr,size_t len,int *port)
{
	return 0;
}
int  
wnLocateAddr(wnFd fd,char * strptr,size_t len,int *port)
{
	return 0;
}
int  
wnWritev(wnFd fd,struct wnBuf *bufs,size_t bufslen)
{
	int nw;
	for(;;){
		if((nw = writev(fd,(struct iovec*)bufs,bufslen)) < 0){
			if(errno == EINTR){
				continue;
			}
			nw = -errno;
		}
		break;
	}
	return nw;
}
int  
wnWrite(wnFd fd,void *buf,size_t sz)
{
	int nw;
	for(;;){
		if((nw = write(fd,buf,sz)) < 0){
			if(errno == EINTR){
				continue;
			}
			nw = -errno;
		}
		break;
	}
	return nw;
}
int  
wnReadv(wnFd fd,struct wnBuf *bufs,size_t bufslen)
{
	int nr;
	for(;;){
		if((nr = readv(fd,(struct iovec*)bufs,bufslen)) < 0){
			if(errno == EINTR){
				continue;
			}
			nr = -errno;
		}
		break;
	}
	return nr;
}
int  
wnReadFull(wnFd fd,void *buf,size_t sz)
{
	return 0;
}
int  
wnRead(wnFd fd,void *buf,size_t sz)
{
	int nr;
	for(;;){
		if((nr = read(fd,buf,sz)) < 0){
			if(errno == EINTR){
				continue;
			}
			nr = -errno;
		}
		break;
	}
	return nr;
}

int  
wnClose(wnFd fd)
{
	return close(fd);
}
int  
wnSetNonBlock(wnFd fd,int flag)
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
wnSetKeepAlive(wnFd fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_KEEPALIVE,&flag,sizeof(flag));
}
int  
wnSetNoDelay(wnFd fd,int flag)
{
	return setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag));
}
int  
wnSetRcvBuf(wnFd fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_RCVBUF,&flag,sizeof(flag));
}
int  
wnSetSndBuf(wnFd fd,int flag)
{
	return setsockopt(fd,SOL_SOCKET,SO_SNDBUF,&flag,sizeof(flag));
} 	