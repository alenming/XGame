#ifndef NewAreaActivityController_h__
#define NewAreaActivityController_h__

#include "Communication/ICommListener.h"

class NewAreaActivityController : public ICommListener
{
public:
	NewAreaActivityController();
	virtual ~NewAreaActivityController();

	//发送获取开服活动主界面指令
	static void sendGetMainInfoMsg();

	//发送领取奖励指令
	static void sendGetRewardMsg(int rewardId);

	static NewAreaActivityController* getInstance();
	static void destroyInstance();

private:

	void parseGetMainInfoMsg(const char* szData);

	void parseGetRewardMsg(const char* szData);

	//解析后台推送消息
	void parseNewAreaActPushMsg(const char* szData);

	virtual void onRecvMessage(int nCmdID, const char* szData);

private:
	static NewAreaActivityController* mInstance;

};


#endif