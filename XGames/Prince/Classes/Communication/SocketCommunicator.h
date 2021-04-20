#ifndef CommCenter_h__
#define CommCenter_h__

#include "Socket/SocketClient.h"
#include "AbstractCommunicator.h"
#include "ICommListener.h"
#include "Utils/Tea/TEACrypt.h"
#include "Widget/LayerWaiting.h"


// 心跳包重试最大次数;
const int MAX_HEART_BEAT_REPLAY = 3;

// 心跳包发送时间间隔
const int HEART_BEAT_INTERVAL = 5;

// 最大重连次数
const int MAX_AUTOREPLAYCOUNT = 3;

//消息超时时间
const int MSG_TIME_OUT = 18;


class SocketCommunicator: public AbstractCommunicator
{

public:

	SocketCommunicator();
	virtual ~SocketCommunicator();

public:

	static SocketCommunicator* getInstance();

	//启动Socket连接
	void startSocket();

	//发送消息
	virtual void sendMessage( int nCmdID, const char* szData );

	// 清理网络连接;
	void  clear();

	//消息接收定时器
	virtual	void update(float dt);

private:

	//关闭Socket连接
	void stopSocket();

	//启动心跳
	void startHeartbeat();

	//关闭心跳
	void closeHeartbeat();

	//心跳定时器
	void heartbeat(float dt);

	//启动重连
	void reconnection();

	// 添加一条消息到缓存队列;
	void MQ_addMessage(int nCmdId, Message* pMsg);

	// 从缓存队列中删除一条消息;
	void MQ_removeMessage(int nCmdId);

	//清除消息缓存
	void clearMsgCache();

	//发送消息缓存
	void sendMsgCache();

	//玩家选择重连的回调;
	void callBackReconnect(Ref* pSender, CustomRetType type);

	//超时回调
	void callBackTimeOut(Ref* pSender, CustomRetType type);

	//启动消息超时定时器
	void startMsgTimeOut();

	//关闭消息超时定时器
	void closeMsgTimeOut();

	//消息超时定时器
	void msgTimeOut(float dt);

private:

	SocketClient* mSocketClient;

	//Tea加解密密钥
	static uint32_t mKey[];
	
	//Tea加密算法
	TEACrypt mTea;

	//重连次数
	short mAutoConnectCount;

	// 消息缓存队列;
	std::map<int, Message*>  mSendMsgCache;

	// 心跳包累积数量;
	int mHeartBeatCount;

	//是否已经连接上
	bool mIsConnected;

	//自动重连标记
	bool mIsAutoReconnect;
};

#endif // CommCenter_h__
