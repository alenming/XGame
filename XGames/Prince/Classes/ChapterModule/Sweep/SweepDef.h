/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		SweepDef
 *  Description:	扫荡数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/17/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef SweepDef_h__
#define SweepDef_h__

#include "ChapterModule/ChapterDef.h"

// 扫荡状态定义;
typedef struct tagSweepState
{
	bool	bSweeping;		// 当前是否正在扫荡;
	int		nChapterId;		// 章节Id;
	int		nBarrierId;		// 关卡Id;
	BARRIER_LEVEL	nLv;	// 难度;
	int		nTotalCount;	// 总共扫荡次数;
	int		nCycStrength;	// 周期(耗体力和时长);
	float   nCycTime;		
	float   nLeftTime;		// 剩余时间;
	int		nFinishTimes;	// 当前已完成次数;

	void  clear()
	{
		bSweeping = false;
		nChapterId = INVALID_CHAPTER_OR_BARRIER_ID;
		nBarrierId = INVALID_CHAPTER_OR_BARRIER_ID;
		nLv = BARRIER_LEVEL_0;
		nTotalCount = 0;
		nCycStrength = 0;
		nCycTime = 0.0f;
		nLeftTime = 0.0f;
		nFinishTimes = 0;
	}

	tagSweepState()
	{
		clear();
	}
}SweepState;


// 扫荡结果定义;
typedef struct tagSweepSettlement
{
	int  nBarrierId;	// 关卡Id;
	int  nOrder;		// 第几次掉落;
	int  nCoin;			// 获得银币;
	int  nExp;			// 获得经验;
	int  nPower;		// 扣除体力;

	// 掉落物品<templateId, count>;
	vector<pair<int, int> > vcDropItems;

	tagSweepSettlement()
		: nBarrierId(-1)
		, nOrder(0)
		, nCoin(0)
		, nExp(0)
		, nPower(0)
	{
		vcDropItems.clear();
	}
}SweepSettlement;

// 扫荡结算展示节点;
typedef struct tagUI_SweepSettlementNode
{
	// 分隔线;
	ImageView*	imgSpace;

	// 标题;
	ImageView*  imgTitle;

	// 货币/经验;
	ImageView*	imgCoin;
	Text*		txtCoin;
	ImageView*	imgExp;
	Text*		txtExp;
	ImageView*  imgPower;
	Text*		txtPower;

	tagUI_SweepSettlementNode()
		: imgSpace(nullptr)
		, imgTitle(nullptr)
		, imgCoin(nullptr)
		, txtCoin(nullptr)
		, imgExp(nullptr)
		, txtExp(nullptr)
		, imgPower(nullptr)
		, txtPower(nullptr)
	{
	}

}UI_SweepSettlementNode;


// 剧情关卡扫荡UI组件集;
typedef struct tagUI_Sweep
{
	Layout*		pRoot;

	// 主面板;
	Layout*		pnlSweep;
	Button*		btnSweepClose;

	ImageView*	imgTitle;
	ImageView*	imgScrollBg;
	ScrollView*	scrollviewSettlement;
	CustomScrollBar*  scrollBar;

	Text*		txtPower;
	Text*		txtEnergy;
	Button*		btnContinue;

	tagUI_Sweep()
		: pRoot(nullptr)
		, pnlSweep(nullptr)
		, btnSweepClose(nullptr)
		, imgTitle(nullptr)
		, imgScrollBg(nullptr)
		, scrollviewSettlement(nullptr)
		, txtPower(nullptr)
		, txtEnergy(nullptr)
		, btnContinue(nullptr)
	{
	}
}UI_Sweep;


#endif // SweepDef_h__
