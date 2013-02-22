
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wod_net.h"
#define BUF_SZ 1024
int main()
{
	unsigned char rdbuf[BUF_SZ+1];
	rdbuf[BUF_SZ]=0;
	wnFd fd = wnTcpListen(TCP4,"0.0.0.0",8199);
	if(fd < 0){
		printf("%s\n", strerror(-fd));
		exit(1);
	}
	printf("%d\n",8199);
	for(;;){
		wnFd cfd = wnAccept(fd);
		if(cfd < 0){
			printf("%s\n", strerror(-cfd));
			exit(1);
		}else{
			int nr = wnRead(cfd,rdbuf,BUF_SZ);
			if(cfd < 0){
				printf("%s\n", strerror(-cfd));
				wnClose(cfd);
				continue;
			}
			rdbuf[nr] = 0;
			printf("%s\n", rdbuf);
			wnWrite(cfd,rdbuf,nr);
			wnClose(cfd);
		}
	}
}
