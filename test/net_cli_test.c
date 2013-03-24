/*
 * net_cli_test.c
 *
 *  Created on: 2013-2-22
 *      Author: wd
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wod_net.h"
int main()
{
	char buf[] = "hello server i`m client!";
	wod_socket_t fd = wod_net_tcp_connect(TCP4,"0.0.0.0",8199);
	if(fd < 0){
		printf("%s\n", strerror(-fd));
		exit(1);
	}
	printf("%d\n",8199);
	wod_net_write(fd,buf,sizeof(buf));
	return 0;
}

