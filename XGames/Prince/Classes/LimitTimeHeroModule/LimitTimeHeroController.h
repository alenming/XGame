/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LimitTimeHeroController.h
 *  Description:	限时神将模块的Controller实现;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			04/11/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef LimitTimeHeroController_h__
#define LimitTimeHeroController_h__

#include "Communication/ICommListener.h"
#include "LimitTimeHeroDataDef.h"

class LimitTimeHeroController : public ICommListener
{
public:
	LimitTimeHeroController();
	~LimitTimeHeroController();

public:

	//拉取活动表格数据
	static void sendGetTableDataMsg();

	//拉取活动主界面数据
	static void sendGetMainInfoMsg();

	//抽卡/宝匣 param.1.抽卡/开宝箱; 2.一次/十次
	static void sendTakeCardMsg(eActType actType, eChouKaType type);

	//领取积分段宝箱奖励 param. 积分
	static void sendGetScoreRewardMsg(int boxId);

private:

	void parseTableDataMsg(const char* szData);

	void parseMainInfoMsg(const char* szData);

	void parseTakeCardMsg(const char* szData);

	void parseScoreRewardMsg(const char* szData);

private:
	virtual void onRecvMessage(int nCmdID, const char* szData);
};

#endif