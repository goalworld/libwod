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
typedef SOCKET wodNetFd;
#else
typedef int wodNetFd;
#endif

enum wodNetTCP{
	TCP4,
	TCP6
};
struct wodNetBuf{
	void * b_body;
	size_t b_sz;
};
wodNetFd wodNetTcpListen(enum wodNetTCP,const char * addr,int port);
wodNetFd wodNetTcpConnect(enum wodNetTCP,const char * addr,int port);
wodNetFd wodNetAccept( wodNetFd fd);
int  wodNetClose(wodNetFd fd);

int  wodNetRemoteAddr(wodNetFd fd,char * strptr,size_t len,int *port);
int  wodNetLocateAddr(wodNetFd fd,char * strptr,size_t len,int *port);
int  wodNetWritev(wodNetFd fd ,struct wodNetBuf *bufs,size_t bufslen);
int  wodNetWrite(wodNetFd fd,void *buf,size_t sz);
int  wodNetReadv(wodNetFd fd,struct wodNetBuf *bufs,size_t bufslen);
int  wodNetRead(wodNetFd fd,void *buf,size_t sz);

int  wodNetReadFull(wodNetFd fd,void *buf,size_t sz);


int  wodNetSetKeepAlive(wodNetFd fd,int flag);//0,1
int  wodNetSetNoDelay(wodNetFd fd,int flag);//0,1
int  wodNetSetNonBlock(wodNetFd fd,int flag);//0,1
int  wodNetSetRcvBuf(wodNetFd fd,int flag);//SIZE
int  wodNetSetSndBuf(wodNetFd fd,int flag);//SIZE

#endif
