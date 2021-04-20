/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ChapterDef
 *  Description:	关卡相关数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/20/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef ChapterDef_h__
#define ChapterDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "Temp/CustomPop.h"
#include "Temp/CustomTips.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/SystemUtils.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


//////////////////////////////////////////////////////////////////////////
// 数据定义;

#define MAX_BARRIER_STAR_COUNT				3			// 关卡可获星级(难度种类)，目前是3个;
#define MAX_STAR_REWARD_TYPE				3			// 星级奖励种类，目前是3个;

#define INVALID_CHAPTER_OR_BARRIER_ID		-1			// 无效的章节或关卡Id;
#define DEFAULT_CHAPTER_ID					10001		// 默认章节Id;
#define DEFAULT_BARRIER_ID					101001		// 默认关卡Id;

#define BARRIER_LIST_NODE_WIDTH				302			// 关卡列表节点宽度;
#define BARRIER_LIST_NODE_HEIGHT			620			// 关卡列表节点高度;
#define BARRIER_LIST_SPACE					-5.0f		// 关卡列表间距;

#define SWEEP_SETTLEMENT_LINE_COUNT			5			// 扫荡结果展示单排显示数量;
#define SWEEP_SETTLEMENT_CHILD_HEIGHT		220.0f

#define STR_BARRIER_FORBIDDEN_STRENGTH		10056
#define STR_BARRIER_FORBIDDEN_LV			10057
#define STR_BARRIER_FORBIDDEN_DAY_COUNT		10058
#define STR_BARRIER_FORBIDDEN_NOT_ARRIVED	10059
#define STR_BARRIER_FORBIDDEN_FR_UNVALIBLE	10060
#define STR_BARRIER_FORBIDDEN_SWEEP_TICKET	10061

// Font;
#define FONT_NAME_FOUNDER_BOLD	SystemUtils::getFontFileName()

// 从其他页面跳转至章节/关卡，跳转类型定义;
enum CHAPTER_JUMP_TYPE
{
	CH_JUMP_TYPE_FIGHT,			// 战斗结束跳转;
	CH_JUMP_TYPE_ELSE			// 其他跳转;
};

// 战斗类型定义;
enum FIGHT_TYPE
{
	FIGHT_TYPE_INVALID,
	FIGHT_TYPE_PVE,					// PVE - 剧情;
	FIGHT_TYPE_PVE_ACTIVITY,		// PVE - 活动;
	FIGHT_TYPE_PVP_ARENA,			// PVP - 竞技场;
	FIGHT_TYPE_MAZE_TRUE,			// PVE - 迷宫真战斗;
	FIGHT_TYPE_ROB_FIGHT,			// PVE|PVP - 掠夺战斗;
	FIGHT_TYPE_TOWER,				// PVP - 爬塔战斗;
	FIGHT_TYPE_TRAIL,				// PVP - 试炼战斗;
	FIGHT_TYPE_GUIDE,				// PVE - 新手引导战斗;
	FIGHT_TYPE_FIND,				// PVE - 寻宝
	FIGHT_TYPE_BOSS,				// PVE - 世界boss
	FIGHT_TYPE_GROUP_BOSS			// PVE - 帮派boss
};

// 章节/关卡类型;
enum CHAPTER_TYPE
{
	CHAPTER_TYPE_STORY		= 0,	// 剧情;
	CHAPTER_TYPE_TOWER		= 1,	// 爬塔;
	CHAPTER_TYPE_EVENT		= 2,	// 活动;
	CHAPTER_TYPE_MAZE		= 3,	// 迷宫;
									// ...
	CHAPTER_TYPE_INVALID	= 99	// 无效值;
};

// 关卡难度级别;
enum BARRIER_LEVEL
{
	BARRIER_LEVEL_0 = 0,			// 普通;
	BARRIER_LEVEL_1,				// 精英;
};

// UI消息类型
enum OBS_PARAM_TYPE_CM
{
	// 剧情章节使用;
	OBS_PARAM_TYPE_CHAPTER_STATE,			// 剧情关卡状态;

	// 剧情关卡使用;
	OBS_PARAM_TYPE_CHAPTER_INFO,			// 剧情章节信息(公共区域，不包含关卡列表);
	OBS_PARAM_TYPE_BARRIER_LIST,			// 剧情关卡列表;
	OBS_PARAM_TYPE_BARRIER_INFO,			// 单个剧情关卡属性展示(遇敌/掉落/通关限制等);
	OBS_PARAM_TYPE_BARRIER_LV,				// 关卡切换难度;
	OBS_PARAM_TYPE_SWEEP_STATE,				// 扫荡状态(倒计时);
	OBS_PARAM_TYPE_SWEEP_SETTLEMENT,		// 扫荡结算;
	OBS_PARAM_TYPE_STAR_REWARD,				// 星级奖励;
	OBS_PARAM_TYPE_STAR_REWARD_RESULT,		// 星级奖励领取结果;

	// 活动关卡使用;
	OBS_PARAM_TYPE_ACTIVITY_LIST,			// 更新活动关卡列表;
	OBS_PARAM_TYPE_ACTIVITY_NODE,			// 更新活动关卡节点;
	OBS_PARAM_TYPE_ACTIVITY_BUY,			// 购买成功;
	OBS_PARAM_TYPE_ACTIVITY_SWEEP			// 扫荡结果;
};

// 章节数据定义(对应csv);
typedef struct tagChapterInfo
{
	// 章节ID;
	int	nChapterId;

	// 章节名称;
	string	strChapterName;

	// 星级奖励<每阶段star数量, <Id, Count> >;
	vector<pair<int, map<int, int> > > vcStarReward;

	// 精英关卡(预留);
	int	nHighRaid;

	// 迷宫关卡(预留);
	int	nMaze;

	// 进入等级;
	int	nLevel;

	// 背景图资源;
	string	strRes;

	// 章节描述;
	string	strDescription;

	// 章节随机宝箱;
	int	nRollReward;

	// 章节关卡<Id>;
	vector<int> vcBarriers;

	tagChapterInfo()
		: nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nHighRaid(0)
		, nMaze(0)
		, nLevel(0)
		, nRollReward(0)
	{
		strChapterName.clear();
		vcStarReward.clear();
		strRes.clear();
		strDescription.clear();
		vcBarriers.clear();
	}

}ChapterInfo;

typedef struct tagChapterPos
{
	// 组Id;
	int nGroupId;

	// 章节Id;
	int nChapterId;

	// 标题资源Id;
	int nTitleResId;

	// x,y坐标;
	float  fPosX;
	float  fPosY;

	tagChapterPos()
		: nGroupId(1)
		, nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nTitleResId(-1)
		, fPosX(0.0f)
		, fPosY(0.0f)
	{}

}ChapterPos;


// 关卡数据定义(对应csv);
typedef struct tagBarrierInfo
{
	// 关卡ID;
	int nBarrierId;

	// 关卡名称;
	string	strBarrierName;

	// 关卡类型;
	CHAPTER_TYPE	nType;

	// 关卡难度;
	BARRIER_LEVEL	nLevel;

	// 进入等级;
	int	nEnterLevel;

	// 扣除体力;
	int	nCost;

	// 怪物Id
	vector<int> vcMonsterId;

	// 关卡掉落;
	vector<int>	vcReward;

	// 关卡奖励经验;
	int	nExp;

	// 关卡奖励银币;
	int	nCoin;

	// 每日通关次数;
	int	nTime;

	// 关卡场景资源;
	int	nResId;

	// 关卡描述;
	string	strDescription;

	// 关卡奖励展示(Id & ResId & Quality);
	vector<int>	vcRewardShow;
	vector<int>	vcRewardShowRes;
	vector<int>	vcRewardShowQua;

	// 关卡怪物展示(Id & ResId);
	vector<int>	vcMonsterShow;
	vector<int>	vcMonsterShowRes;

	// 首次通关奖励<templateId, Count>;
	map<int, int>  mapFirstReward;

	// 关卡标签页展示的怪物Id;
	int nBossShowId;

	// 最高回合;
	int	nMaxRound;

	// 可增值通关上限;
	int	nMaxTime;

	//////////// 以下是活动关卡配置; //////////
	// 开启时间;
	int  nStartDate;

	// 关闭时间;
	int  nEndDate;

	// 具体开启的日期标识(以周为单位，例如1011011);
	bitset<7>  btOpenDay;

	// 活动关卡序号;
	unsigned int  nOrder;

	// 标题资源;
	int  nTitleId;

	//////////// 爬塔奖励特例; //////////
	// 奖励预览<Id, Count>;
	map<int, int>  mapRewardShow;

	tagBarrierInfo()
		: nBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nType(CHAPTER_TYPE_INVALID)
		, nLevel(BARRIER_LEVEL_0)
		, nEnterLevel(0)
		, nCost(0)
		, nExp(0)
		, nCoin(0)
		, nTime(0)
		, nResId(0)
		, nBossShowId(0)
		, nMaxRound(0)
		, nMaxTime(0)
		, nStartDate(0)
		, nEndDate(0)
		, nOrder(0)
		, nTitleId(0)
	{
		strBarrierName.clear();
		vcMonsterId.clear();
		vcReward.clear();
		strDescription.clear();
		vcRewardShow.clear();
		vcRewardShowRes.clear();
		vcRewardShowQua.clear();
		vcMonsterShow.clear();
		vcMonsterShowRes.clear();
		mapFirstReward.clear();
		mapRewardShow.clear();
	}

}BarrierInfo, ActivityBarrierInfo;


// 关卡进度数据;
typedef struct tagFightRecordBarrier
{
	// 关卡Id;
	int  nBarrierId;

	// 当日闯关次数;
	int  nDayCount;

	// 当日已重置次数;
	int  nResetDayCount;

	// 已获星级(0/1/2/3);
	int  nStar;

	tagFightRecordBarrier()
		: nBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nDayCount(0)
		, nResetDayCount(0)
		, nStar(0)
	{
	}

	tagFightRecordBarrier(int nId)
		: nBarrierId(nId)
		, nDayCount(0)
		, nResetDayCount(0)
		, nStar(0)
	{
	}

}FR_Barrier;


// 章节进度数据;
typedef struct tagFightRecordChapterStory
{
	// 是否已通关全部章节;
	bool bIsGameOver;

	// 当前进行的章节Id;
	int  nCurChapterId;

	// 当前进行的关卡Id;
	int  nCurBarrierId;

	// 星级奖励已领取次数<ChapterId, <1/2/3, true/false> >;
	map<int, map<int, bool> >  mapStarRewardCount;

	// 当前已进行的关卡信息;
	map<int, map<int, FR_Barrier> >  mapPrevBarrier;

	size_t size()
	{
		return mapPrevBarrier.size();
	}

	tagFightRecordChapterStory()
		: bIsGameOver(false)
		, nCurChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nCurBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
	{
		mapStarRewardCount.clear();
		mapPrevBarrier.clear();
	}

}FR_ChapterStory;


//////////////////////////////////////////////////////////////////////////

// FrameUI组件集;
typedef struct tagUI_Frame
{
	Layout*			pRoot;

	///// 前景层(控制按钮相关);
	Layout*			pnlFrame;

	// 索引;
	Button*			btnChapterIndex[5];

	tagUI_Frame()
	{
		memset(this, 0, sizeof(tagUI_Frame));
	}

}UI_Frame;


// 剧情章节节点;
typedef struct tagUI_ChapterNode
{
	// 关卡按钮;
	ImageView*		imgChapter;

	// 关卡标题图片;
	ImageView*		imgTitle;

	// 通关标识;
	//ImageView*	imgPass;

	// 星星;
	ImageView*		imgStar;

	// 星级奖励;
	ImageView*		imgStarReward;

	// 进度;
	ImageView*		imgCountBg;
	TextAtlas*		labCur;
	ImageView*		imgSlash;
	TextAtlas*		labTotal;

	tagUI_ChapterNode()
	{
		memset(this, 0, sizeof(tagUI_ChapterNode));
	}

}UI_ChapterNode;


// 剧情章节UI组件集;
class ChapterNode;
typedef struct tagUI_Chapter
{
	Layout*			pRoot;
	ScrollView*		scrollViewMap[7];
	ScrollView*		scrollViewBuilding;
	ImageView*		imgLight;

	// 章节列表;
	map<int, ChapterNode*>  mapChapterNode;

	tagUI_Chapter()
		: pRoot(nullptr)
		, scrollViewBuilding(nullptr)
		, imgLight(nullptr)
	{
		mapChapterNode.clear();
		for (unsigned int i = 0; i < 7; i++)
			scrollViewMap[i] = nullptr;
	}

}UI_Chapter;


// 剧情关卡节点;
typedef struct tagUI_BarrierNode
{
	// 底图;
	ImageView*		imgBg;

	// 关卡名;
	ImageView*		imgBarrierName;
	Text*			txtBarrierName;

	// 边框(简单/普通/困难);
	ImageView*		imgFrame;

	//////// 以下为可选内容;

	// 高光选中;
	ImageView*		imgSelected;

	// 禁用;
	ImageView*		imgDisabled;
	ImageView*		imgLock;

	// 掉落碎片;
	ImageView*		imgPieceTitle;
	UIToolHeader*	imgPiece;

	// boss展示;
	ImageView*		imgBoss;

	// 通关标识;
	ImageView*		imgPass;

	// 新开启标识;
	ImageView*		imgNew;

	// 关卡已获星级;
	//ImageView*		pImgStar[MAX_BARRIER_STAR_COUNT];

	tagUI_BarrierNode()
		: imgBg(nullptr)
		, imgBarrierName(nullptr)
		, txtBarrierName(nullptr)
		, imgFrame(nullptr)
		, imgSelected(nullptr)
		, imgDisabled(nullptr)
		, imgLock(nullptr)
		, imgPieceTitle(nullptr)
		, imgPiece(nullptr)
		, imgBoss(nullptr)
		, imgPass(nullptr)
		, imgNew(nullptr)
	{
	}
}UI_BarrierNode;


// 剧情关卡UI组件集;
class BarrierNode;
typedef struct tagUI_Barrier
{
	Layout*			pRoot;

	///// 关卡;
	Layout*			pnlBarrier;
	Button*			btnBack;

	// 关卡列表;
	ScrollView*				scrollviewBarrierList;

	// 关卡难度;
	Button*		btnNormal;
	Button*		btnHard;

	// 关卡信息展示;
	ImageView*  imgBarrierInfoBg;
	Text*		txtBarrierName;
	Text*		txtBarrierDesc;
	ImageView*	imgDropLine;
	Text*		txtCoin;
	Text*		txtPower;
	Text*		txtExp;
	Text*		txtDayCountCur;
	Text*		txtDayCountTotal;

	// 扫荡和战斗;
	Button*		btnSweep1;
	Button*		btnSweep10;
	Button*		btnFight;

	// 星级奖励;
	TextAtlas*		numLab1;
	TextAtlas*		numLab2;
	Button*			btnBox[3];
	TextAtlas*		numStage[3];
	LoadingBar*		progressBar;

	// 星级奖励领取面板;
	Layout*			pnlStar;
	ImageView*		imgStarContentBg;
	Layout*			pnlStarContent;
	TextAtlas*		numLab3;

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


	tagUI_Barrier()
	{
		memset(this, 0, sizeof(tagUI_Barrier));
	}
}UI_Barrier;

//////////////////////////////////////////////////////////////////////////

// 章节UI数据;
typedef struct tagUI_CHAPTER_STORY_PARAM
{
	// 当前进行的章节Id;
	int		nCurChapterId;

	// 章节状态<Id, <Star/totalStar> >;
	map<int, vector<int> >	mapChapterState;

	// 星级奖励领取状态<1/2/3, true/false>;
	map<int, bool>  mapStarRewardState;

	tagUI_CHAPTER_STORY_PARAM()
		: nCurChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
	{
		mapChapterState.clear();
		mapStarRewardState.clear();
	}
}UI_CHAPTER_STORY_PARAM;


// 关卡列表 - 单个关卡数据;
typedef struct tagUI_BARRIER_PARAM
{
	// 章节Id;
	int  nChapterId;

	// 关卡Id;
	int  nBarrierId;

	// 关卡难度;
	BARRIER_LEVEL nLv;

	// 关卡名称;
	string  strBarrierName;

	// 是否boss关;
	bool bIsBoss;

	// boss图片;
	int  nBossShowId;

	// 图片;
	int  nResId;

	// 已获星级(0/1/2/3);
	int  nStarCount;

	// 首次通关奖励<templateId, Count>;
	map<int, int>  mapFirstReward;

	// 是否可入;
	bool bIsEnabled;

	tagUI_BARRIER_PARAM()
		: nChapterId(0)
		, nBarrierId(0)
		, nLv(BARRIER_LEVEL_0)
		, bIsBoss(false)
		, nBossShowId(0)
		, nResId(0)
		, nStarCount(0)
	{
		strBarrierName.clear();
		mapFirstReward.clear();
		bIsEnabled = false;
	}
}UI_BARRIER_PARAM;


// 关卡列表数据;
typedef struct tagUI_BARRIER_LIST_PARAM
{
	// 章节Id;
	int  nChapterId;

	// 本章已开启的最高难度;
	BARRIER_LEVEL nMaxLv;

	// 关卡难度;
	BARRIER_LEVEL  nLv;

	// 本章内的当前进行关卡(若有);
	int  nCurBarrierId;

	// 需要选中的关卡(若有);
	int  nSelectedBarrierId;

	// 需要显示新开启标识的关卡(若有);
	int  nNewBarrier;

	// 关卡列表;
	vector<UI_BARRIER_PARAM>  vcBarrierList;

	tagUI_BARRIER_LIST_PARAM()
		: nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nMaxLv(BARRIER_LEVEL_0)
		, nLv(BARRIER_LEVEL_0)
		, nCurBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nSelectedBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nNewBarrier(INVALID_CHAPTER_OR_BARRIER_ID)
	{
		vcBarrierList.clear();
	}
}UI_BARRIER_LIST_PARAM;


// 关卡UI公共数据;
typedef struct tagUI_CHAPTER_DETAIL_PARAM
{
	// 章节名称;
	int		nChapterId;
	string  strChapterName;

	// 玩家全部金币;
	int  nTotalCoin;

	// 玩家全部体力;
	int  nTotalStrength;

	// 已开放难度;
	BARRIER_LEVEL  nMaxLv;

	// 新开启;
	bool		   bNewLv;
	BARRIER_LEVEL  nNewLv;

	tagUI_CHAPTER_DETAIL_PARAM()
		: nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nTotalCoin(0)
		, nTotalStrength(0)
		, nMaxLv(BARRIER_LEVEL_0)
		, bNewLv(false)
		, nNewLv(BARRIER_LEVEL_0)
	{
		strChapterName.clear();
	}
}UI_CHAPTER_DETAIL_PARAM;


// 星级奖励UI数据;
typedef struct tagUI_STAR_REWARD_PARAM
{
	// 章节Id;
	int		nChapterId;

	// 当前进度;
	int  nStarCount;
	int  nTotalStarCount;

	// 领取状态;
	map<int, bool>  mapState;

	tagUI_STAR_REWARD_PARAM()
		: nChapterId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nStarCount(0)
		, nTotalStarCount(0)
	{
		for (int i = 0; i < 3; ++i)
		{
			mapState.insert(make_pair(i+1, false));
		}
	}
}UI_STAR_REWARD_PARAM;


// 具体关卡数据;
typedef struct tagUI_BARRIER_INFO_PARAM
{
	bool bIsAllowed;			// 是否可入;
	int	 nBarrierId;			// 关卡Id;
	string  strBarrierName;		// 关卡名称;
	CHAPTER_TYPE  nType;		// 关卡类型;
	int  nPower;				// 当前体力;
	int  nConsume;				// 体力消耗;
	int  nCoin;					// 获得银币;
	int  nExp;					// 获得经验;
	int  nDayCount;				// 挑战次数;
	int  nTotalCount;
	int  nStar;					// 是否过关(星数);
	string	strDescription;		// 关卡描述;
	int  nBossShowResId;		// 标签页怪物展示;

	// 掉落<Id, <ResId, Quality...> >;
	map<int, vector<int> >	mapRewardShow;

	// 遇敌;
	vector<int>	vcMonsterShowRes;

	tagUI_BARRIER_INFO_PARAM()
		: bIsAllowed(false)
		, nBarrierId(INVALID_CHAPTER_OR_BARRIER_ID)
		, nType(CHAPTER_TYPE_INVALID)
		, nPower(0)
		, nConsume(0)
		, nCoin(0)
		, nExp(0)
		, nDayCount(0)
		, nTotalCount(0)
		, nStar(0)
		, nBossShowResId(0)
	{
		strBarrierName.clear();
		strDescription.clear();
		mapRewardShow.clear();
		vcMonsterShowRes.clear();
	}
}UI_BARRIER_INFO_PARAM;

#endif // ChapterDef_h__
