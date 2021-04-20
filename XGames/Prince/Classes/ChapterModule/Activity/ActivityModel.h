/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ActivityModel
 *  Description:	活动关卡数据模型的实现;
 *					1. 活动数据存储/解析;
 *					2. 更新UI
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/06/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef ActivityModel_h__
#define ActivityModel_h__

#include "ActivityDef.h"
#include "Utils/Observer.h"

class ActivityModel : public Observable
{
public:
	ActivityModel(void);
	~ActivityModel(void);

	// singleton;
	static ActivityModel*	getInstance();
	static void  destroyInstance();

	// 更新活动数据;
	void  clearActivityBarrierInfo() { m_mapActivityBarrierInfo.clear(); };
	void  updateActivityBarrierInfo(ActivityBarrierInfo info);

	// 查询活动关联的关卡数据;
	void  getActivityBarrierByActivityId(int nActivityId, vector<ActivityBarrierInfo*>& vcBarrierInfo);

	// 更新活动列表;
	void  updateActivityList(map<int, ActivityState> mapState);

	// 更新活动状态;
	void  updateActivityState(ActivityState state);
	void  getActivityState(int nActivityId, ActivityState& state);

	// 查询活动消耗体力;
	int   getPowerCost(int nActivityBarrierId);

	// 获取/设置当前选中的活动Id和活动关卡Id;
	void  setCurActivityBarrierId(int nActivityId, int nActivityBarrierId);
	int   getCurActivityId() { return m_nCurSelActivityId; };
	int   getCurActivityBarrierId() { return m_nCurSelActivityBarrierId; };

	// 获取/设置当前选中的活动关卡类型;
	ACTIVITY_TYPE  getCurActivityType() { return m_nCurSelActivityType; };
	void  setCurActivityType(ACTIVITY_TYPE type) { m_nCurSelActivityType = type; };

	// 获取当前活动关卡配置的英雄匹配列表(阵型模块用到);
	void  getActivityHeroList(vector<pair<int, string> >& vcActivityHeroList);

	// 关卡是否过关;
	bool  isActivityBarrierPassed(int nActivityId, int nActivityBarrierId);

	// 关卡是否可进入;
	int   isActivityBarrierValid(int nActivityId, int nActivityBarrierId);
	int   isActivityBarrierValid(int nActivityId, ActivityBarrierInfo activityInfo);

	// 购买成功;
	void  onBuySuccess();

	// 更新扫荡结果;
	void  updateSweepResult(map<int, int>* mapReward);

	// VIP相关;
	int  getNextValidVip(int& nNextValue);
	int  getCurVipLeftTimes(int nActivityId);
	int  getCurMaxTimes();
	int  getBuyActivityCost(int nActivityId);

private:

	// 更新至UI;
	void  updateUI(OBS_PARAM_TYPE_CM type, void* data);

private:

	static ActivityModel*  m_pInstance;

	// 活动数据(CSV配置);
	map<int, ActivityBarrierInfo>  m_mapActivityBarrierInfo;

	// 活动状态(服务器下发);
	map<int, ActivityState>  m_mapActivityState;

	// 当前选中的活动关卡Id;
	int  m_nCurSelActivityId;
	int  m_nCurSelActivityBarrierId;
	ACTIVITY_TYPE m_nCurSelActivityType;
};

#endif // ActivityModel_h__
