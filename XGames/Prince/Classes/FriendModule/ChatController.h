#ifndef ChatController_h__
#define ChatController_h__

#include "IChatView.h"
#include "Communication/ICommListener.h"
#include "FriendDef.h"
#include "Utils/Observer.h"

class ChatController: public ICommListener,public Observable
{
public:
	ChatController();
	~ChatController();

	static ChatController* getInstance();
	static void destroyInstance();
	
	//消息接收接口
	virtual	void onRecvMessage(int nCmdID, const char* szData);

	void processNoticeMessageData(const char* szData); //跑马灯
	void processPlayerBehaviorMessageData(const char* szData); //玩家行为消息
	void processCSChatMessageData(const char* szData); //客户端发的消息
	void processSCPushMessageData(const char* szData); //服务器推送的消息

	//设置UI接口
	void onSetView(IChatView* pView);
	void onSetNoticeView(INoticeView* pView);
	//移除UI接口
	void onRemoveView();
	void onRemoveNoticeView();
public:
	void onSendMessage(const int nuserId, const int ntype, const string strMessage);
	
protected:
	
	IChatView* m_pView;
	INoticeView* m_pNoticeView;

	static ChatController* ms_pInstance;

};

#endif 
