/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ActivityDef
 *  Description:	活动关卡数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/17/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef ActivityDef_h__
#define ActivityDef_h__

#include "ChapterModule/ChapterDef.h"

// 活动关卡类型;
enum ACTIVITY_TYPE
{
	ACT_TYPE_CHIP_JiangHuChuanWen = 1,	// 江湖传闻;
	ACT_TYPE_EXP_JiuJingKaoYan,			// 酒经考验;
	ACT_TYPE_COIN_LiangShanMiCang		// 梁山秘藏;
};

// 活动状态数据;
typedef struct tagActivityState 
{
	int nActivityId;	// 活动Id;
	int nBuyCount;		// 当前已购买次数;
	int nValidCount;	// 当前可进入次数;
	int nTotalCount;	// 总次数;

	// 关卡记录<Id, Star>;
	map<int, int>  mapBarrierRecord;

	tagActivityState()
		: nActivityId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nBuyCount(0)
		, nValidCount(0)
		, nTotalCount(0)
	{
		mapBarrierRecord.clear();
	}
}ActivityState;

// 活动关卡节点;
typedef struct tagUI_ActivityNode
{
	Layout*		pnlRoot;	
	ImageView*	imgBg;		// 背景图;
	ImageView*	imgFrame;	// 选中边框;
	Text*		txtCount;	// 剩余次数;
	Button*		btnEnter;	// 进入关卡;
	Text*		txtTips;	// 提示文本;

	tagUI_ActivityNode()
	{
		memset(this, 0, sizeof(tagUI_ActivityNode));
	}
}UI_ActivityNode;


// 活动关卡节点数据;
typedef struct tagUI_ACTIVITY_NODE_PARAM
{
	int				nActivityId;
	ACTIVITY_TYPE	nType;			// 活动类型;
	vector<int>		vcEnemy;		// 敌方阵容;
	vector<int>		vcDrops;		// 掉落;
	int				nValidCount;	// 可挑战次数;
	int				nTotalCount;	// 挑战次数上限;

	tagUI_ACTIVITY_NODE_PARAM()
		: nActivityId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nType(ACTIVITY_TYPE::ACT_TYPE_COIN_LiangShanMiCang)
		, nValidCount(0)
		, nTotalCount(0)
	{
		vcEnemy.clear();
		vcDrops.clear();
	}
}UI_ACTIVITY_NODE_PARAM;


// 活动关卡UI组件集;
typedef struct tagUI_Activity
{
	Layout*		pRoot;
	Layout*		pnlActivity;

	// 难度选择面板;
	Layout*		pnlDifficulty;
	ImageView*	imgDifficultyBg;
	Layout*		pnlContent;
	Button*		btnDifficultyClose;
	Text*		txtCostPower;

	// VIP状态;
	Layout*		pVipStateRoot;
	ImageView*	imgVipStateBg;
	ImageView*	imgCurVip;
	Text*		txtVipStateTips1;
	Text*		txtVipStateTips2;
	Text*		txtVipStateTips3;
	Text*		txtCost;
	Text*		txtCurCount;
	Button*		btnVipStateOK;
	Button*		btnVipStateCancel;
	Button*		btnVipStateClose;

	// VIP提升;
	Layout*		pVipRoot;
	ImageView*	imgVipBg;
	ImageView*	imgVipNext;
	Text*		txtVipTips1;
	Text*		txtVipTips2;
	Text*		txtNextCount;
	Button*		btnVipOK;
	Button*		btnVipCancel;
	Button*		btnVipClose;

	tagUI_Activity()
	{
		memset(this, 0, sizeof(tagUI_Activity));
	}
}UI_Activity;

typedef struct tagUI_ActivityBarrierNode
{
	Layout*		pRoot;
	ImageView*	imgTitle;
	Layout*		pnlBg;
	Text*		txtLevel;
	ImageView*	imgMask;
	Button*		btnSweep;
	Button*		btnFrame;
	ImageView*	imgBoss;

	tagUI_ActivityBarrierNode()
	{
		memset(this, 0, sizeof(tagUI_ActivityBarrierNode));
	}
}UI_ActivityBarrierNode;

#endif // ActivityDef_h__
