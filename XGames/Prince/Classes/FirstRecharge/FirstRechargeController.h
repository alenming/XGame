#ifndef FirstRechargeController_h__
#define FirstRechargeController_h__

#include "Communication/ICommListener.h"

class FirstRechargeController : public ICommListener
{
public:

	FirstRechargeController();
	virtual ~FirstRechargeController();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

	//发送领取首充指令
	static void sendGetRewardMsg();

private:

	//解析首充奖励领取
	void parseGetRewardMsg(const char* szData);
};

#endif