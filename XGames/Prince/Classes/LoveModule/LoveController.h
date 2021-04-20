/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LoveController
 *  Description:	情缘控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#include "LoveHeader.h"
#include "LoveModel.h"

#ifndef LoveController_h_
#define LoveController_h_

class LoveController:public ICommListener
{
public:

	LoveController();
	~LoveController();

	virtual void onRecvMessage(int nCmdID, const char* szData) override;

	static void getHeroListInfo(); //拉取英雄列表信息
	static void sendGift(int petId,int itemId,int count = 1);//送礼
	static void buyGift(int itemId,int amount);//购买
	static void fingerGuess(int id,FINGER_ENTER_TYPE type); //猜拳
	static void playMap(int petId); //游玩地图

	static void chooseMap(int petId,int mapId); //选择游玩地图
	static void mapEvent(int petId,int mapId,int curCoords,int tarCoords,int _event);//地图节点游玩

	static void buyGuessCountInfo(int petId); //获取购买猜拳信息
	static void buyGuessCount(int petId); //获取购买猜拳次数

	static void changeGuess(int petId,int type); //修改猜拳结果
	static void getGuseePrize(int petId,int idx); //领取奖励


	static void addAttribute(int petId,int type); //增加属性
	static void resetAttribute(int petId); //重置属性

	void processHeroListInfoData(const char* szData);//解析英雄列表
	void processHeroSendGiftData(const char* szData);//解析送礼
	void processHeroBuyGiftData(const char* szData);//解析买礼
	void processHeroFingerGuessData(const char* szData);//解析猜拳
	void processHeroPlayMapData(const char* szData);//解析游玩拉取地图
	void processChooseMapData(const char* szData);//解析游玩地图节点信息
	void processChooseMapEvent(const char* szData);//解析游玩地图上节点
	void processGuessCountInfo(const char* szData);//解析购买猜拳信息
	void processGuessCount(const char* szData);//解析购买猜拳次数
	void processAddAttribute(const char* szData); //解析增加属性点
	void processResetAttribute(const char* szData);//解析重置属性点
	void processChangeGuess(const char* szData);//解析修改猜拳结果
	void processGetGuessPrize(const char* szData); //领取猜拳奖励
};


#endif