/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		TowerModel
 *  Description:	爬塔数据模型;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/21/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef TowerModel_h__
#define TowerModel_h__

#include "TowerDef.h"
#include "Utils/Observer.h"
#include "HeroModule/HeroModel.h"
#include "ChapterModule/ChapterDef.h"

class TowerModel : public Observable
{
public:
	~TowerModel(void);

	static TowerModel* getInstance();

	static void destroyInstance();

	// (试炼)更新主界面信息;
	void  updateTrailMainInfo(TrailMainInfo info);

	// (试炼)查询刷新宝藏花费金币数量;
	int   getRefreshTreasureCost();// { return m_queTreasureRefreshData.front(); };

	// (试炼)查询buff信息;
	void  getBuffInfo(int nBuffId, string& name, string& desc);

	// (爬塔/试炼)查询某一层的战斗类型;
	TOWER_FIGHT_TYPE  getFightType(TOWER_TYPE nTowerType, int nFloor);

	// (试炼)查询遇敌ResId;
	int   getOpResId(TOWER_TYPE type, int nFloor);

	// (试炼)记录/取消buff触发状态;
	void  setBuffTriggered(bool  bIsTriggered);
	bool  isBuffTriggered() { return m_bTriggerBuff; };

	// (试炼)查询当前实际楼层;
	int   getTrailCurFloor();

	// (试炼/爬塔)查询是否当前楼层;
	bool  isLogicCurFloor(TOWER_TYPE nType, int nFloor);

	// (试炼)更新当前楼层;
	void  updateTrailFloor();

	// (试炼)查询试炼主界面信息;
	TrailMainInfo* getTrailMainInfo() { return &m_trailMainInfo; };

	// (试炼)更新buff数据;
	void  updateBuff(int nFloor);
	void  updateBuffContent(vector<pair<int, int> >  vcBuff) { m_vcBuff = vcBuff; };

	// (试炼)查询buff价格;
	int   getBuffPrice(int nBuffId);

	// (试炼)更新buff状态;
	void  updateBuffState(int nBuffId, bool bUsed = true);

	// (试炼)更新星星数量;
	void  updateStarCount(int nCount, map<string, float>  mapBuff);

	// (试炼)更新金币数量;
	void  updateGold(int nGold);

	// (试炼)查询/设置试炼积分;
	void  setScore(int nScore);// { m_trailMainInfo.nScore = nScore; };
	int   getScore() { return m_trailMainInfo.nScore; };

	// (试炼)查询buff生效范围;
	int   getBuffRange(int nBuffId);

	// (试炼)判定buff领取状态(是否在条件内领取完成);
	bool  isBuffAllApplied();

	// (试炼)记录/取消宝藏触发状态;
	void  setTreasureTriggered(int  nTreasureBoxId) { m_nTreasureBoxId = nTreasureBoxId; };
	bool  isTreasureTriggered() { return (m_nTreasureBoxId != -1); };
	int   getTreasureBoxId() { return m_nTreasureBoxId; };

	// (试炼)首次更新宝藏数据;
	void  firstUpdateTreasure();

	// (试炼)查询宝藏开启次数;
	int   getTreasureOpendTimes();// { return 6-m_queTreasureOpenData.size(); };

	// (试炼)查询宝藏下次开启消耗金币数量;
	int   getNextTreasureCost();// { return m_queTreasureOpenData.front(); };

	// (试炼)宝藏开启完成;
	void  onTreasureOpend(int nTreasureId, int nCount);

	// (试炼)检查开启单个宝藏操作是否已完成;
	vector<int>   isTreasureOpend();

	// (试炼)刷新宝藏;
	bool  refreshTreasure(int nBoxId, bool bfirst = false);

	// (试炼)更新对手信息;
	void  updateTrailOp(map<int, TrailOp> mapTrailOp);
	void  updateTrailOpState(int nValidLv);

	// (试炼)检查楼层是否越界;
	bool  checkTrailFloorIsValid(int nFloor);

	// (试炼)宝箱开启结果;
	void  updateTrailReward(map<int, int> mapReward);

	// (试炼)重置宝藏数据;
	void  resetTreasureData();

	// (试炼)选定对手等级;
	void  selectOpLv(int nOpLv) { m_nSelectedOpLv = nOpLv; };
	int   getSelectedOpLv() { return m_nSelectedOpLv; };

	// (试炼)获取上次重置日期;
	//int   getTrailResetDate() { return m_trailMainInfo.nTrailResetDate; };

	// (试炼)更新楼层;
	void  updateTrailCurFloorNum(int delta = 1); //{ m_trailMainInfo.nCurFloor += delta; };

	// (试炼)重置;
	void  resetTrail(int date);

	// (试炼)获取可加buff的英雄列表;
	void  getBuffDstHeros(vector<HeroParam*>& vcDstHeroList);

	// (试炼/爬塔)更新排行榜;
	void  updateRanking(TOWER_TYPE nType, const UI_TRAIL_RANKING_PARAM& uiParam);

	// (爬塔)更新主界面信息;
	void  updateTowerMainInfo(TowerMainInfo info);

	// (爬塔)查询购买挑战次数的价格;
	//int   getPrice() { return m_towerMainInfo.nPrice; };

	// (爬塔)重置;
	void  resetTower();

	// (爬塔)检查楼层是否越界;
	bool  checkTowerFloorIsValid(int nFloor);

	// (爬塔)获取上次重置日期;
	//int   getTowerResetDate() { return m_towerMainInfo.nTowerResetDate; };

	// (爬塔)更新当前楼层;
	void  updateTowerFloor();

	// (爬塔)楼层/关卡Id转换;
	int   towerFloor2Id(int nFloor);
	int   towerId2Floor(int nBarrierId);

	// (爬塔)查询通关条件;
	string  getClearCondition(int nFloor);

	// (爬塔)更新/获取所有楼层关卡信息;
	void  updateTowerFloorInfo(map<int, BarrierInfo> mapInfo);
	map<int, BarrierInfo>*  getTowerFloorInfo() { return &m_mapBarrierTower; };

	// (爬塔)查询所有进度信息;
	TowerMainInfo*  getTowerMainInfo() { return &m_towerMainInfo; };

	// (爬塔)查询当前层数/关卡Id;
	int   getTowerCurFloor() { return m_towerMainInfo.nCurFloor; };
	int   getTowerCurBarrierId() { return towerFloor2Id(m_towerMainInfo.nCurFloor); };

	// (爬塔)查询历史最高楼层;
	int   getTowerMaxFloor() { return m_towerMainInfo.nMaxFloor; };

	// (爬塔)更新奖励预览信息;
	void  updateTowerRewardPreview();

	// (爬塔)查询单次扫荡时间;
	int   getSweepSingleTime();

	// (爬塔)扫荡倒计时;
	void  onCountDown(int nSec);

	// (爬塔)扫荡结果;
	void  updateSweepSettlement(TowerSweepSettlement* result);
	void  checkUISweepSettlement();

	// (爬塔)是否在扫荡中;
	void  setTowerSweepState(bool bIsSweeping) { m_bIsTowerSweeping = bIsSweeping; };
	bool  getTowerSweepState() { return m_bIsTowerSweeping; };

	// ;
	int   getNextValidVip(int& nNextValue);
	int   getCurVipLeftTimes();
	int   getCurMaxTimes();
	int   getTowerResetCost();

private:
	TowerModel(void);

	// 更新至UI;
	void  updateUI(OBS_PARAM_TYPE_TOWER type, void* data);

	// (试炼)更新buff状态;
	void  updateUIBuffState();

	// (试炼)构造宝藏数据结构;
	void  constructTreasureParam(int nBoxId, multimap<int, int>& mapInfo);

private:
	// singleton
	static TowerModel*  m_pInstance;

	// 试炼主界面信息;
	TrailMainInfo  m_trailMainInfo;

	// buff触发状态;
	bool  m_bTriggerBuff;

	// 爬塔是否在扫荡中;
	bool  m_bIsTowerSweeping;

	// 当前触发的buff列表<buffId, price>;
	vector<pair<int, int> >  m_vcBuff;

	// 当前buff列表的兑换状态<id, sold>;
	map<int, bool>  m_mapBuffState;

	// 当前触发的宝藏组合boxId(包含触发状态判断);
	int  m_nTreasureBoxId;

	// 当前开启的单个宝藏Id;
	int  m_nTreasureId;
	int  m_nTreasureCount;

	// 宝藏(单个，共6个每组)开启次数;
	//int  m_nTreasureOpendTimes;

	// 试炼对手;
	map<int, TrailOp>  m_mapTrailOp;

	// 选定的对手等级;
	int  m_nSelectedOpLv;

	// 爬塔主界面信息;
	TowerMainInfo  m_towerMainInfo;

	// 爬塔楼层配置(CSV);
	map<int, BarrierInfo>  m_mapBarrierTower;

public:

	// 试炼宝藏【刷新】次数及价格;
	queue<int> m_queTreasureRefreshData;

	// 试炼宝藏【开启】次数及价格;
	queue<int> m_queTreasureOpenData;
	queue<int> m_queTreasureOpenData_backup;
};

#endif // TowerModel_h__
