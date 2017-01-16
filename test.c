#include "socket_server.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void *
_poll(void * ud) {
	struct socket_server *ss = ud;
	struct socket_message result;
	for (;;) {
		int type = socket_server_poll(ss, &result, NULL);
		// DO NOT use any ctrl command (socket_server_close , etc. ) in this thread.
		switch (type) {
		case SOCKET_EXIT:
			return NULL;
		case SOCKET_DATA:
			printf("message(%u) [id=%d] size=%d\n",result.opaque,result.id, result.ud);
			free(result.data);
			break;
		case SOCKET_CLOSE:
			printf("close(%u) [id=%d]\n",result.opaque,result.id);
			break;
		case SOCKET_OPEN:
			printf("open(%u) [id=%d] %s\n",result.opaque,result.id,result.data);
			break;
		case SOCKET_ERROR:
			printf("error(%u) [id=%d]\n",result.opaque,result.id);
			break;
		case SOCKET_ACCEPT:
			printf("accept(%u) [id=%d %s] from [%d]\n",result.opaque, result.ud, result.data, result.id);
			break;
		}
	}
}

#if 1
static void
test(struct socket_server *ss) {
	pthread_t pid;
	pthread_create(&pid, NULL, _poll, ss);

	int c = socket_server_connect(ss,100,"127.0.0.1",80);
	printf("connecting %d\n",c);
	int l = socket_server_listen(ss,200,"127.0.0.1",8888,32);
	printf("listening %d\n",l);
	socket_server_start(ss,201,l);
	int b = socket_server_bind(ss,300,1);
	printf("binding stdin %d\n",b);
	int i;

	for (i=0;i<100;i++) {
		c = socket_server_connect(ss, 400+i, "127.0.0.1", 8888);
        	//socket_server_send(ss, c, (char *)"sendBuf", sizeof("sendBuf"));
	}


	sleep(5);
	socket_server_exit(ss);

	pthread_join(pid, NULL); 
}

int
main() {
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, 0);

	struct socket_server * ss = socket_server_create();
	test(ss);
	socket_server_release(ss);

	return 0;
}

#endif



#if 0


#include "socket_server.h"  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <pthread.h>  
#include <string.h>  
  
static void *   
_poll(void *ud){  
    struct socket_server *ss = ud;  
    struct socket_message result;  
    for (;;) {  
        int type = socket_server_poll(ss, &result, NULL);  
        switch (type) {  
        case SOCKET_EXIT:  
            return NULL;  
        case SOCKET_DATA:  
            printf("message(%lu) [id=%d] size=%d\n",result.opaque,result.id, result.ud);  
            free(result.data);  
            break;  
        case SOCKET_CLOSE:  
            printf("close(%lu) [id=%d]\n",result.opaque,result.id);  
            break;  
        case SOCKET_OPEN:  
            printf("open(%lu) [id=%d] %s\n",result.opaque,result.id,result.data);  
            break;  
        case SOCKET_ERROR:  
            printf("error(%lu) [id=%d]\n",result.opaque,result.id);  
            break;  
        case SOCKET_ACCEPT:  
            printf("accept(%lu) [id=%d %s] from [%d]\n",result.opaque, result.ud, result.data, result.id);  
            break;  
        }  
    }  
socket_server_exit(struct socket_server *)  
    return 0;  
}  
  
int main(void){  
    struct socket_server *ss = socket_server_create();  
    //连接服务器(非堵塞)，获得服务器的id  
    int conn_id = socket_server_connect(ss, 100, "127.0.0.1", 8888);  
    pthread_t pid;  
    pthread_create(&pid, NULL, _poll, (void*)ss);  
    char buf[1024] = {0};  
    //监听键盘输入  
    while(fgets(buf, sizeof(buf), stdin) != NULL){  
        if (strncmp(buf, "quit", 4) == 0){  
            break;  
        }  
        buf[strlen(buf) - 1] = '\n';  
        char *sendbuf = (char*)malloc(sizeof(buf)+1);  
        memcpy(sendbuf, buf, strlen(buf)+1);  
        //将输入发送到指定ID的服务端  
        socket_server_send(ss, conn_id, sendbuf, strlen(sendbuf));  
    }  
    //退出循环而退出子线程  
    socket_server_exit(ss);  
    socket_server_release(ss);  
}  
  
服务器  
#include "socket_server.h"  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
  
int main(void){  
    struct socket_server *ss = socket_server_create();  
    int listen_id = socket_server_listen(ss, 100, "", 8888, 32);  
    //启动socket服务，开始监听来自客户的请求  
    socket_server_start(ss, 200, listen_id);  
  
    struct socket_message result;  
    for (;;) {  
        int type = socket_server_poll(ss, &result, NULL);  
        switch (type) {  
        case SOCKET_EXIT:  
            goto EXIT_LOOP;  
        case SOCKET_DATA:  
            printf("message(%lu) [id=%d] size=%d\n",result.opaque,result.id, result.ud);  
            socket_server_send(ss, result.id, result.data, result.ud);  
            break;  
        case SOCKET_CLOSE:  
            printf("close(%lu) [id=%d]\n",result.opaque,result.id);  
            break;  
        case SOCKET_OPEN:  
            printf("open(%lu) [id=%d] %s\n",result.opaque,result.id,result.data);  
            break;  
        case SOCKET_ERROR:  
            printf("error(%lu) [id=%d]\n",result.opaque,result.id);  
            break;  
        case SOCKET_ACCEPT:  
            printf("accept(%lu) [id=%d %s] from [%d]\n",result.opaque, result.ud, result.data, result.id);  
            //当接收到客户端的链接请求后，调用start开始监听，对于accept, ud是新连接的id;   
            socket_server_start(ss, 300, result.ud);  
            break;  
        }  
    }  
EXIT_LOOP:  
    //销毁  
    socket_server_release(ss);  
    return 0;  
}  



#endif









