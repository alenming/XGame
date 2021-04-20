#ifndef _NET_DEF_H_
#define _NET_DEF_H_


//消息发送缓冲区大小
#define SEND_BUFFER_SIZE 1024*64

//消息接受缓冲区大小
#define RECEIVE_BUFFER_SIZE 1024*64

//连接超时秒数
#define CONNECT_TIMEOUT_SECONDS 6

//发送select超时秒数
#define THREAD_SEND_TIMEOUT_SECONDS 1

//接收select超时秒数
#define THREAD_RECV_TIMEOUT_SECONDS 1


#endif //_NET_DEF_H_