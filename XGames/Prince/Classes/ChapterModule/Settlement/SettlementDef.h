/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		SettlementDef
 *  Description:	结算数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/17/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef SettlementDef_h__
#define SettlementDef_h__

#include "ChapterModule/ChapterDef.h"

// 战斗结算数据定义;
typedef struct tagSettlementInfo
{
	FIGHT_TYPE  nFightType;	// 战斗类型;
	int  nBarrierId;		// 关卡Id;
	bool bWin;				// 输赢;
	int nStarCount;			// 星级;
	int  nPower;			// 体力;
	int  nEnergy;			// 精力;
	int  nTotalGold;		// 当前金币总量;
	int  nTotalCoin;		// 当前银币总量;
	int  nCoin;				// 本次获得银币数量;
	int  nExp;				// 本次获得经验;
	int  nScore;			// 本次获得积分;
	int  nPkgSize;			// 背包大小;
	int  nPkgUseSize;		// 背包当前已用量;

	// 关卡掉落<templateId, count>;
	map<int, int>  mapReward;

	// 各级别星级奖励是否可领取[0/1];
	int nStarReward[MAX_STAR_REWARD_TYPE];

	// 升级相关;
	bool bIsLevelUp;
	int  nPreEnergy;
	int  nPrePower;
	int  nCostPower;
	int  nCostEnergy;

	// 掠夺是否成功;
	bool bRobDone;

	// 活动关卡完成度;
	float fCompletionPercent;

	// 爬塔战斗失败原因;
	string strReason;

	tagSettlementInfo()
		: nFightType(FIGHT_TYPE::FIGHT_TYPE_INVALID)
		, nBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
		, bWin(false)
		, nStarCount(0)
		, nPower(0)
		, nEnergy(0)
		, nTotalGold(0)
		, nTotalCoin(0)
		, nCoin(0)
		, nExp(0)
		, nScore(0)
		, nPkgSize(0)
		, nPkgUseSize(0)
		, bIsLevelUp(false)
		, nPreEnergy(0)
		, nPrePower(0)
		, nCostPower(0)
		, nCostEnergy(0)
		, bRobDone(false)
		, fCompletionPercent(0.0f)
		, strReason("")
	{
		mapReward.clear();
		for (int i = 0; i < MAX_STAR_REWARD_TYPE; i++)
			nStarReward[i] = 0;
	}
}SettlementInfo;


#endif // SettlementDef_h__
