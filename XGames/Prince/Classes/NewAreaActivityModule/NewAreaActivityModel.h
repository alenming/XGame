/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		NewAreaActivityLayer
 *  Description:	新区活动数据;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			26/10/2015
 *  Others: 
 *
 ******************************************************************/

#ifndef NewAreaActivityModel_h__
#define NewAreaActivityModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "NewAreaActivityDataDef.h"
#include "Utils/LocalTimer.h"

class NewAreaActivityModel : public Observable, public TimeStampListener
{
public:
	static NewAreaActivityModel* getInstance();
	static void destroyInstance();

public:

	//本地数据初始化
	void initFromLocal();

	//从服务器拉取过来的数据
	void initFromSvr(const Json::Value& actData);

	//从服务器推送了消息
	void updateFromSvr(const Json::Value& actData);

	//是否从服务器拉取过数据
	bool getIsLoaded();

	//设置是否主界面第一次拉取
	void setIsFristLoad(bool isFirstLoad);

	//获取是否有可领取的奖励
	bool isHaveCanGetReward();

	//获取活动是否是开启状态
	bool getActStates();
	//设置活动开启状态
	void setActStates(bool states);

	//记录当前购买的特价礼包信息
	//获取当前购买的特价礼包信息
	//清空当前购买的特价礼包信息(因为和其他任务奖励是同一条ID，使用完必须清空)
	void setCurBuyBox(sTeJiaBox tjBox);
	sTeJiaBox getCurBuyBox();
	void clearCurBuyBox();

	virtual void timeOut();

private:
	NewAreaActivityModel();
	virtual ~NewAreaActivityModel();

public:
	vector<sNewAreaActivityData> m_vecNewAreaActData;
	//倒计时时间，给UI用
	int m_iRemainTime;

	//当前是开服第几天
	int m_iCurActDay;

	//是否拉取过数据
	bool m_bIsLoaded;

	//是否是主界面第一次拉取
	bool m_bIsFirstLoad;

	//活动结束时间戳
	int m_iActEndedTime;

	//活动是否开启
	bool m_bIsActOpen;

	//当前购买的特价礼包信息
	sTeJiaBox m_sTeJiaBox;

private:

	static NewAreaActivityModel* ms_pInstance;
};

#endif