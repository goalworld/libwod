#include <wod_thread.h>
#include <wod_ev.h>
static int worker(void *arg);
int 
main(int argc,char *argv[])
{
	
}
typedef cmd{
	int type;
	int fd;
}cmd_t;
typedef struct thread_ctx{
	vod_event_main_t * ev_main;
	int bstop;
	wod_mutext_t *mtx;
	wod_queue_t *que;
}thread_ctx_t;
typedef struct buf{

}buf_t;
typedef struct chain{
	buf_t buf;	
	struct chain * next;
}chain_t;
typedef struct connect{
	wod_socket_t sock;
	chain_t * head;
	chain_t * tail;
}connect_t;
static int
worker(void *arg)
{
	thread_ctx_t *ctx = (thread_ctx_t *)arg;	
	ctx->ev_main = wod_event_main_new(10240,WV_POLL_EPOLL);
	cmd_t cmd;
	for(;;){
		wod_mutex_lock(ctx->mtx);
		if( wod_queue_pop(ctx->que,&cmd) != 0){
			cmd
		}
		wod_mutex_unlock(ctx->mtx);
		wod_event_main_once(ctx->ev_main);
	}
}
static void 
_do_read(wod_event_main_t*loop,void *nv,int mask)
{

}
static void
_do_write(vod_event_main_t*loop,void *nv,int mask)
{

}
static void 
_do_accept(wod_event_main_t *loop,void *nv,int mask)
{

}
