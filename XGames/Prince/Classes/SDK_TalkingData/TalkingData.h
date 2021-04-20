/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		talkingData
 *  Description:	(C++)talkingData的标准实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			11/26/2015
 *  Others:			
 *
 ******************************************************************/

#ifndef TalkingData_h__
#define TalkingData_h__

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "cocos2d.h"
USING_NS_CC;
#endif

#include "Communication/EnvConfig.h"
using namespace std;



typedef std::map< std::string, std::string > EventParamMap;

class TalkingData
{
public:

	TalkingData(void);
	~TalkingData(void);

public:

	//登录的时候调用（设置玩家id 游戏区服）
	static void loginTalkingData(string userID, string SeverId,string accountName);

	//玩家等级变化
	static void setLevel(int level);
	
	//赠予虚拟币(5, “新手奖励”)
	static void onReward(double virtualCurrencyAmount, string reason = "");

	//记录付费点(“头盔”, 2个, 25单价)
	static void onPurchase(string item, int itemNumber,double priceInVirtualCurrency);

	//消耗物品或服务等(“红药”, 1个)
	static void onUse(string item, int itemNumber);

	//接受或进入副本(“洛兰之森”);
	static void onBegin(string missionId);

	//完成副本(“洛兰之森”);
	static void onCompleted(string missionId);

	//失败副本及原因(“洛兰之森”， “quit”);
	static void onFailed(string missionId, string cause = "");

	//自定义事件
	static void onEvent(string eventId, EventParamMap map);

};

#endif