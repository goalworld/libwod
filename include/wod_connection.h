/*
 * wod_connection.h
 *
 *  Created on: 2013-3-7
 *      Author: wd
 */

#ifndef WOD_CONNECTION_H_
#define WOD_CONNECTION_H_
#include "wod_type.h"
#include "wod_net.h"
#include "wod_event.h"
typedef struct wod_connection wod_connection_t;
typedef void (*wod_connection_read_cb)(wod_connection_t *con);
wod_ret_t wod_connection_connect(wod_connection_t ** ppcon,const char *ip,int port);
wod_ret_t wod_connection_create(wod_connection_t ** ppcon, wod_socket_t socket);
wod_ret_t wod_connection_write(wod_connection_t * con,void *buf,size_t sz);
wod_ret_t wod_connection_read(wod_connection *con,void *buf,size_t sz);
wod_ret_t wod_connection_close(wod_connection *con);
wod_ret_t wod_connection_destroy(wod_connection *con);

wod_ret_t wod_connection_resume(wod_event_t *ev,wod_connection_read_cb cb);
wod_ret_t wod_connection_pause(wod_event_t *ev);
#endif /* WOD_CONNECTION_H_ */
