/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSignHelp
 *  Description:	每日签到模块说明的Model实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			06/10/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef LayerSignModel_h__
#define LayerSignModel_h__

#include "cocos2d.h"
#include "Utils/Observer.h"
#include "DataManager/DataManager.h"
#include "Utils/StringFormat.h"
#include "Communication/Json/features.h"
#include "Communication/Json//value.h"
#include "LayerSignDef.h"
#include "HeroModule/HeroModel.h"
#include "ToolModule/ToolModel.h"
#include "MainModule/MainModel.h"
#include "ToolModule/ToolModel.h"
#include "Temp/PopReward.h"


USING_NS_CC;
using namespace std;

class LayerSignParam;
class LayerSignModel : public Observable 
{
public:
	virtual ~LayerSignModel(void);

	//活动单例
	static LayerSignModel* getInstance();
	static void destroyInstance();

	//解析奖励下拉列表数据
	void parseSignData(const Json::Value &val);

	//解析奖励领取反馈数据
	void parseSignGetData(const Json::Value &val);

	//通知监听者
	void notify(SIGN_EVENT_TYPE eventType, void* data = nullptr);

	Vector<LayerSignParam*>* getSignParam() { return &mLayerSignParam;}
	int& getSignIdx1() {return signIdx1;};
	int& getSignIdx2() {return signIdx2;};

	//Vector<LayerSignParam*>* getSignParam7() { return &mLayerSignParam7;}

	SignRewardStateInfo getSignRewardStateInfo() { return mSignState; }
	int&  getShowHeroId() {return showHeroId;};
	int&  getShowHeroCount() {return showHeroCount;};
	int&  getPrize() {return prize;};
	int&  getPetId() {return petId;};

	ReWard_Show getRewardShow() { return rewardShow; }

	bool getIsLoad() {return isLoad;};
	time_t& getServerTime() {return mServerTime;}; //服务器时间
private:
	LayerSignModel(void);

	static LayerSignModel* mInstance;

	Vector<LayerSignParam*> mLayerSignParam;
	//七日先不要
	//Vector<LayerSignParam*> mLayerSignParam7;

	SignRewardStateInfo mSignState; 
	ReWard_Show rewardShow;//展示奖励
	time_t mServerTime; //服务器时间

	bool isLoad;
	int  signIdx1;
	int  signIdx2;
	int  showHeroId;
	int  showHeroCount;
	int  prize;
	int petId; // -1 是没有整卡 	有的话就是卡牌的ID
};

class LayerSignParam : public Ref
{
public:
	LayerSignParam(): 
	signCondition(-1),
	vipDouble(0),   
	vipLevel(-1),   
	rewardBet(-1),
	stat(SIGN_STAT_TYPE_CAN)
	{

	}

	virtual ~LayerSignParam(){ };

	static LayerSignParam* create()
	{
		LayerSignParam* _LayerSignParam = new LayerSignParam();
		_LayerSignParam->autorelease();
		return _LayerSignParam;
	};

public:


public:
	int  signCondition;//累计签到次数
	SignRewardInfo signReward; //物品奖励

	int  vipDouble;    //是否开启双倍
	int	 vipLevel;     //Vip等级条件
	int  rewardBet;    //vip奖励倍数
	SIGN_STAT_TYPE  stat;         //状态1可签 2已签 3可补签  4 vip达到可领状态（只限当天）
};
#endif


