/******************帮派boss协议通信**********************/

#ifndef GroupBossController_h__
#define GroupBossController_h__

#include "Communication/ICommListener.h"
#include "GroupBossDataDef.h"

class GroupBossController : public ICommListener
{
public:
	GroupBossController();
	virtual ~GroupBossController();

	static GroupBossController* getInstance();
	static void destroyInstance();

public:
	//发送获取主界面信息
	static void sendGetMainInfoMsg();

	//发送获取排名数据
	static void sendGetRankMsg(eRankType type);

	//发送鼓舞指令
	static void sendInspireMsg();

	//发送领取击退boss宝箱 param1. 领取方式， param2. 单领时id
	static void sendGetBossRewardMsg(eGetRewardMethod method, int boxId = 0);

	//发送领取伤害奖励宝箱
	static void sendGetDamRewardMsg(int boxId);

	//发送锦囊兑换奖励
	static void sendGetJinNangRewardMsg(int jinNangId);

public:
	//解析主界面信息
	void parseMainInfoMsg(const char* szData);

	//解析排名数据
	void parseRankMsg(const char* szData);

	//解析鼓舞指令
	void parseInspireMsg(const char* szData);

	//解析领取击退boss宝箱奖励
	void parseBossRewardMsg(const char* szData);

	//解析领取伤害奖励宝箱
	void parseDamRewardMsg(const char* szData);

	//解析锦囊兑换奖励
	void parseJinNangRewardMsg(const char* szData);
	
	//推送活动开启结束消息
	void parseOpenStatePushMsg(const char* szData);

	//解析领取伤害奖励返回的信息
	void parseGetRewardBackInfo(const char* szData);

	//解析兑换返回信息
	void parseExchangeJnang(const char* szData);

	virtual void onRecvMessage(int nCmdID, const char* szData);

private:
	static GroupBossController* ms_pInstance;

};

#endif