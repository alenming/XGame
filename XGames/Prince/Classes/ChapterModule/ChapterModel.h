/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ChapterModel
 *  Description:	关卡模块 - 章节数据模型的实现;
 *					1. 章节数据存储/解析;
 *					2. 更新UI
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/21/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef ChapterModel_h__
#define ChapterModel_h__

#include "ChapterDef.h"
#include "Sweep/SweepDef.h"
#include "Utils/Observer.h"

class ChapterModel : public Observable
{
public:
	~ChapterModel(void);

	// singleton;
	static ChapterModel* getInstance();
	static void destroyInstance();

	bool  isInit() { return m_bIsInit; };

	// 更新章节配置数据;
	void  updateChapterInfo(map<int, ChapterInfo>  mapInfo);
	void  updateChapterPosInfo(map<int, ChapterPos>  mapPosInfo);

	// 更新/获取进度数据;
	void  updateFightRecord(FR_ChapterStory fightRecord);
	void  updateFightRecordFromBarrier(int nBarrierId, int nStarCount, int nTimes = 1);
	void  getFightRecordData(FR_ChapterStory& fightRecord);

	// 更新剧情关卡UI状态;
	void  updateChapterStoryBase();
	void  updateChapterStoryState();

	// 关卡是否可入;
	// @bFuzzy: 模糊判断;
	//    true:不判断体力/挑战次数等具体标准，只判断关卡是否打开;
	//    false:全部条件都判断;
	bool  isBarrierAllowed(int nChapterId, int nBarrierId, int nCount, string& strReason, bool bFuzzy = false);

	// 更新章节关卡公共信息展示;
	void  updateChapterDetail(int nChapterId);

	// 更新章节难度选择;
	void  selectBarrierLevel(BARRIER_LEVEL nLv);

	// 更新关卡列表;
	void  updateBarrierList(UI_BARRIER_LIST_PARAM& uiParam);

	// 更新/获取扫荡状态数据;
	void  updateSweepState(SweepState sweepData);
	SweepState*  getSweepState() { return &m_sSweepState; };

	// 更新扫荡结果;
	void  updateSweepSettlement(vector<SweepSettlement> vcSettlement);

	// 获取章节内关卡数量;
	int   getBarrierCount(int nChapterId);
	int   getBarrierCountWithLv(int nChapterId, BARRIER_LEVEL nLv);

	// 领取星级奖励结果;
	void  updateGetStarRewardResult(int nChapterId, int nStarIndex);

	// 更新星级奖励;
	void  updateStarReward(int nChapterId);

	// 查询/重置本章新开启难度状态;
	bool  getChapterNewLv(int nChapterId);
	void  resetChapterNewLv(int nChapterId);

	// 查询关卡是否通过;
	bool  isBarrierPassed(int nBarrierId);

	// 查询章节坐标;
	Vec2  getChapterPos(int nChapterId);

	// 获取每一级星级奖励的内容，返回所需的星数量;
	int   getStarRewardsByLv(int nChapterId, int nLv, map<int, int>& mapRewards);

	// 查询某一级的星级奖励是否已领取;
	bool  isGetStarReward(int nChapterId, int nStarIndex);

	// 查询扫荡相关VIP等级;
	int  getVipLvBySweepFunc();

	// 关卡剩余重置次数VIP相关;
	int   getCurMaxTimes();
	int   getBarrierTotalTimes(int nBarrierId);
	int   getBarrierLeftTimes(int nBarrierId);
	int   getNextValidVip( int& nNextValue );

	// 更新关卡重置次数;
	void updateBarrierResetCount(int nBarrierId);

	// 音乐;
	int   getMusicNameIndex(int nChapterId);

	// 跳转类型;
	void  setJumpType(CHAPTER_JUMP_TYPE  jumpType);
	CHAPTER_JUMP_TYPE  getJumpType() { return m_jumpType; };

	////////////////////////////////// Id规则相关; ////////////////////////////////////

	// 获取章节序号10001;
	int   getChapterOrderFromChapterId(int nChapterId) { return nChapterId%1000; };
	int   getChapterOrderFromBarrierId(int nBarrierId) { return (nBarrierId/1000)%100; };

	// 由章节序号构造章节Id;
	int   getChapterIdFromChapterOrder(int nChapterOrder) { return 10000 + nChapterOrder; };

	// 由关卡Id构造章节Id; 101001
	int   constructChapterId(int nBarrierId) { return (nBarrierId/1000)%100 + 10000; };

	// 解析关卡难度;
	BARRIER_LEVEL  getBarrierLv(int nBarrierId);// { return (BARRIER_LEVEL)((nBarrierId % 10000)/1000); };

	// 获取章节内第一关卡;
	int   getFirstBarrierInChapter(int nChapterId) { return 100000 + (nChapterId%1000)*1000 + 1; };

	// 构造关卡难度Id;
	//int   constructBarrierIdWithLv(int nBarrierId, BARRIER_LEVEL nLv) { return ((nBarrierId/10000)*10000 + (nBarrierId%1000)) + nLv*1000; };

	// 获取关卡序号;
	int   getBarrierOrder(int nBarrierId) { return nBarrierId % 1000; };

	// 获取关卡在本难度内的逻辑序号;
	int   getBarrierLogicOrder(int nBarrierId);

	// 解析每一关卡的基础Id(最低难度);
	//int   getBasicBarrierId(int nBarrierId) { return (nBarrierId/10000)*10000 + (nBarrierId%1000); };

	// 获取本关下一个(N个)难度;
	//int   getNextLvInBarrier(int nBarrierId, int N = 1);

	// 获取下一关卡Id;
	bool  getNextBarrier(int nCurBarrierId, int& nNextBarrierId);

private:

	ChapterModel(void);

	// 更新至UI;
	void  updateUI(OBS_PARAM_TYPE_CM type, void* data);

	// 打开关卡(返回值=是否第一次开启);
	bool  openBarrier(int nBarrierId);

	// 检查当前关卡的数值;
	void  checkCurChapterAndBarrier(int nBarrierId);

	
private:

	static ChapterModel* mInstance;

	// csv配置的章节数据;
	map<int, ChapterInfo>   m_mapChapterInfoList;

	// 章节坐标;
	map<int, ChapterPos>	m_mapChapterPos;

	// 剧情章节进度;
	FR_ChapterStory			m_sFightRecord;

	// 扫荡进度;
	SweepState				m_sSweepState;

	// 扫荡结果;
	vector<SweepSettlement>	m_vcSweepSettlement;

	// 星级奖励进度(0, 1, 2, 3);
	//UI_STAR_REWARD_PARAM	m_uiStarRewardRecord;
	map<int, UI_STAR_REWARD_PARAM> m_mapStarRewardRecord;

	// 各章节新开启难度状态;
	map<int, bool> m_mapChapternewLv;

	// 是否已经初始化(主要是战斗记录);
	bool  m_bIsInit;

	// 跳转类型;
	CHAPTER_JUMP_TYPE  m_jumpType;
};

#endif // ChapterModel_h__