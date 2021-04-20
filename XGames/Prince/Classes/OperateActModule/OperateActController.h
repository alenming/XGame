/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		OperateActController
 *  Description:	OperateAct控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			12/7/2015
 *  Others:			
 *
 ******************************************************************/
#include "OperateActHeader.h"
#include "OperateActModel.h"

#ifndef OperateActController_h_
#define OperateActController_h_

class OperateActController:public ICommListener
{
public:
	OperateActController();
	~OperateActController();
	static OperateActController* getInstance();
	static void destroyInstance();

	virtual void onRecvMessage(int nCmdID, const char* szData) override; //接收消息反馈

	void getOprateSuppleMentTable(); //获取运营活动补充表
	void getOprateActInfo(); //获取运营活动信息
	void getOprateActtask(int taskId); //领取奖励
	void BuyOprateFinancial(); //购买理财通
	void BuyVipGift(int taskId,int price); //购买超值礼包

	void processOprateSuppleMentTable(const char* szData);//获取运营活动补充表
	void processOprateActInfo(const char* szData);//获取运营活动信息
	void processOprateActPrize(const char* szData);//获取运营活动奖励
	void processOprateFinancial(const char* szData);//理财通
	void processVipGift(const char* szData);//超值礼包

	static OperateActController* mInstance;

	int m_buyVipGiftPrice;
};


#endif