
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wod_net.h"
#define BUF_SZ 1024
int main()
{
	unsigned char rdbuf[BUF_SZ+1];
	rdbuf[BUF_SZ]=0;
	wod_socket_t fd = wod_net_tcp_listen(TCP4,"0.0.0.0",8199);
	if(fd < 0){
		printf("%s\n", strerror(-fd));
		exit(1);
	}
	printf("%d\n",8199);
	for(;;){
		wod_socket_t cfd = wod_net_accept(fd);
		if(cfd < 0){
			printf("%s\n", strerror(-cfd));
			exit(1);
		}else{
			int nr = wod_net_read(cfd,rdbuf,BUF_SZ);
			if(cfd < 0){
				printf("%s\n", strerror(-cfd));
				wod_net_close(cfd);
				continue;
			}
			rdbuf[nr] = 0;
			printf("%s\n", rdbuf);
			wod_net_write(cfd,rdbuf,nr);
			wod_net_close(cfd);
		}
	}
}
