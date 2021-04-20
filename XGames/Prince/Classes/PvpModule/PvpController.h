/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		PvpController
 *  Description:	Pvp控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/30/2015
 *  Others:			
 *
 ******************************************************************/
#include "PvpHeader.h"
#include "PvpModel.h"

#ifndef PvpController_h_
#define PvpController_h_

class PvpController:public ICommListener
{
public:
	PvpController();
	~PvpController();

	//活动单例
	static PvpController* getInstance();
	static void destroyInstance();

	virtual void onRecvMessage(int nCmdID, const char* szData) override; //接收消息反馈

	//发送
	void getPvpInfo(); //主界面拉取竞技数据
	void getRank(); //竞技场排名
	void getPvpPrize(int idx);//领取竞技场奖励
	void buyPvpCount(int times); //竞技场购买次数

	//解析
	void processPvpInfoData(const char* szData);  //解析拉取数据
	void processpvpRankData(const char* szData); //解析竞技场排名
	void processPvpPrizeData(const char* szData); //解析竞技场奖励
	void processPvpCount(const char* szData); //解析竞技场购买次数


private:
		static PvpController* mInstance;
};


#endif