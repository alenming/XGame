#ifndef BossController_h__
#define BossController_h__

#include "Communication/ICommListener.h"

class BossController : public ICommListener
{
public:
	BossController();
	virtual ~BossController();

public:

	//发送获取boss主界面消息
	static void sendGetBossMainMsg();

	//发送鼓舞消息
	static void sendInspireMsg();

	//发送清除冷却时间消息
	static void sendClearCoolTimeMsg();

	//发送领取奖励消息
	static void sendGetRewardMsg(int rewardId);

	//发送获取伤害排名消息
	static void sendGetDamageRankMsg();

private:

	//解析boss主界面消息
	void parseBossMainMsg(const char* szData);

	//解析鼓舞消息
	void parseInspireMsg(const char* szData);

	//解析清除冷却时间消息
	void parseClearCoolTimeMsg(const char* szData);

	//解析领取奖励消息
	void parseGetRewardMsg(const char* szData);

	//解析伤害排名消息
	void parseGetDamageRankMsg(const char* szData);

	virtual void onRecvMessage(int nCmdID, const char* szData);
};

#endif