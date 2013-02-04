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
typedef SOCKET wnFd;
#else
typedef int wnFd;
#endif

enum wnTCP{
	TCP4,
	TCP6
};
struct wnBuf{
	void * b_body;
	size_t b_sz;
};
wnFd wnTcpListen(enum wnTCP,const char * addr,int port);
wnFd wnTcpConnect(enum wnTCP,const char * addr,int port);
wnFd wnAccept( wnFd fd);

int  wnRemoteAddr(wnFd fd,char * strptr,size_t len,int *port);
int  wnLocateAddr(wnFd fd,char * strptr,size_t len,int *port);
int  wnWritev(wnFd fd ,struct wnBuf *bufs,size_t bufslen);
int  wnWrite(wnFd fd,void *buf,size_t sz);
int  wnReadv(wnFd fd,struct wnBuf *bufs,size_t bufslen);
int  wnRead(wnFd fd,void *buf,size_t sz);

int  wnReadFull(wnFd fd,void *buf,size_t sz);

int  wnClose(wnFd fd);
int  wnSetKeepAlive(wnFd fd,int flag);
int  wnSetNoDelay(wnFd fd,int flag);
int  wnSetNonBlock(wnFd fd,int flag);
int  wnSetRcvBuf(wnFd fd,int flag);
int  wnSetSndBuf(wnFd fd,int flag);

#endif