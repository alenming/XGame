/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		BarrierModel
 *  Description:	关卡模块 - 关卡数据模型的实现;
 *					1. 关卡数据模型;
 *					2. 更新UI
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/21/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef Barrier_Model_h__
#define Barrier_Model_h__

#include "ChapterDef.h"
#include "Utils/Observer.h"
#include "ChapterModel.h"

class BarrierModel : public Observable
{
public:
	~BarrierModel(void);

	// singleton
	static BarrierModel* getInstance();
	static void destroyInstance();

	//////////////////////////////////// 剧情关卡; //////////////////////////////////////
	// 更新剧情关卡配置;
	void  updateBarrierStoryInfo(BarrierInfo info);
	void  clearBarrierStoryInfo();

	// 更新剧情关卡信息展示;
	void  updateBarrierStoryShow(int nBarrierId, FR_Barrier* fightRecord);

	// 查询剧情关卡;
	BarrierInfo* getBarrierStoryById(int nBarrierId);

	//////////////////////////////////// 通用; //////////////////////////////////////

	// 更新至UI;
	void  updateUI(OBS_PARAM_TYPE_CM type, void* data);

	// 查询单次扫荡时长;
	float getSweepTime();

	// 查询付费扫荡单价;
	int   getSweepPrice();

	// 设置数据解析完毕的标识，避免进入PVE重复解析CSV;
	void  setLocalDataReady() { m_bIsLocalDataReady = true; };
	bool  isLocalDataReady()  { return m_bIsLocalDataReady; };

	// 查询单次重置消耗;
	int  getBarrierResetCost(int nBarrierId);
	int  getCurVipLeftTimes(int nBarrierId);


private:

	BarrierModel(void);

private:

	static BarrierModel* mInstance;

	// csv配置的剧情关卡;
	map<int, map<int, BarrierInfo> >  m_mapBarrierStoryList;

	// 本地CSV数据是否已读取;
	bool m_bIsLocalDataReady;

	// 单次扫荡时长;
	float  m_nSweepTime;

};

#endif // Barrier_Model_h__
