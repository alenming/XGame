#ifndef _CDATA_NETSOCKET_H_
#define _CDATA_NETSOCKET_H_

#include "ByteBuffer.h"
#include "message.h"
#include <string.h>
#include <stdlib.h>
#include <queue>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <Winsock2.h>
#include <Wininet.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "pthread.h"

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#endif

using namespace std;

enum SOCKET_CONNECT_STATE
{
	CONNECTED = 0, //已经建立连接
	TO_CONNECT,	   //建立Socket后等待连接
	CONNECTING,	   //正在连接
	NOT_CONNECTED  //未连接
};


class SocketClient{

public:

	SocketClient();
	~SocketClient();
	
public:

	//初始化Socket连接
	void startSocket(std::string host, int port);

	//销毁线程并关闭Socket连接
	void clear();

	//获取Socket连接状态
	SOCKET_CONNECT_STATE getConnectState();
	
	//发送数据
	void pushSendMessage(Message* msg);
	
	//取出接收到的数据
	Message* popReceivedMessage();

	//发送线程/接收线程执行方法
	void threadSend();
	void threadReceive();

	//网络是否可用
	bool isNetAvailable();
	
private:

	//关闭Socket连接
	void closeSocket();

	//销毁所有线程
	void stopThreads();

	//线程休眠
	void sleepThread(long milli);

	//连接服务器，采用非阻塞方式连接
	void connectSocket();

	//接受线程/发送线程静态方法
	static void* ThreadReceiveMessage(void *p);
	static void* ThreadSendMessage(void *p);

	//获取错误码
	int getErrno();

private:

	//连接状态
	SOCKET_CONNECT_STATE mConnectState;

	//线程是否被创建
	bool mThreadRecvCreated;
	bool mThreadSendCreated;

	//是否销毁线程
	bool mSendThreadFinished;
	bool mRecvThreadFinished;

	//Socket连接地址结构体
	sockaddr_in  mSockAddr;
	sockaddr_in6 mSockAddr_v6;

	// 当前服务端使用地址类型;
	unsigned short mCUR_ADDRESS_FAMILY;

	//SocketId
	int mSocketId;

	//发送和接收缓冲区
	ByteBuffer mRecvBuffer;
	ByteBuffer mSendBuffer;

	//接收消息队列
	queue<Message*> mReceivedMessageQueue;
	//发送消息队列
	queue<Message*> mSendMessageQueue;

	//接收线程
	pthread_t mThreadReceive;
	//发送线程
	pthread_t mThreadSend;

	//发送队列同步锁
	pthread_mutex_t mSendQueueMutex;
	//接受队列同步锁
	pthread_mutex_t mRecvQueueMutex;

	//条件锁
	pthread_mutex_t mThreadCondMutex;
	pthread_cond_t mThreadCond;


    
};


#endif
