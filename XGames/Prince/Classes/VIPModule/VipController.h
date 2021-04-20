#ifndef VipController_h__
#define VipController_h__

#include "Communication/ICommListener.h"

class VipController : public ICommListener
{
public:
	VipController();
	virtual ~VipController();

	static VipController* getInstance();
	static void destroyInstance();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData);

	//发送获取主界面信息
	static void sendGetVipListMsg();

	//发送获取vip获得的相关道具数据
	static void sendGetVipBuyMsg(int vipId);

	//发送获取订单号获得的相关道具数据
	static void sendGetOrderId(int productId);

private:

	//解析拉取的VIP界面数据
	void parseVipGetListMsg(const char* szData);
	
	//解析VIP对应的数据
	void parseVipBuyMsg(const char* szData);

	//解析订单号对应的数据
	void parseOrderIdMsg(const char* szData);

private:
	static VipController* ms_pInstance;
};

#endif