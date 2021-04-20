/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		TowerDef
 *  Description:	爬塔相关数据定义;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			11/13/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef TowerDef_h__
#define TowerDef_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include <string>
#include <vector>
#include <map>
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "Temp/CustomPop.h"
#include "FormationModule/CustomScrollBar.h"
#include "RankNode.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;


// 功能类型;
enum TOWER_TYPE
{
	TOWER_TYPE_INVALID,			//
	TOWER_TYPE_TOWER,			// 爬塔;
	TOWER_TYPE_TRAIL			// 试炼;
};

enum POP_WND_TYPE
{
	POP_WND_TYPE_OP,			// 选择对手;
	POP_WND_TYPE_BUFF			// 星之佑护;
};

enum R_WND_TYPE
{
	R_WND_TYPE_RANKING,			// 试炼排行榜;
	R_WND_TYPE_REWARD			// 试炼奖励预览;
};

enum TOWER_FIGHT_TYPE
{
	TFT_FIGHT,					// 战斗(之后可能有buff);
	TFT_REWARD					// 宝箱(之后可能有宝藏);
};

enum TRAIL_EVENT_TYPE
{
	TET_INVALID,				// 空;
	TET_BUFF,					// buff;
	TET_TREASURE				// 宝藏;
};

// UI消息类型
enum OBS_PARAM_TYPE_TOWER
{
	OBS_PARAM_TYPE_TRAIL_MAIN_INFO,				// 试炼主界面信息;
	OBS_PARAM_TYPE_TOWER_MAIN_INFO,				// 爬塔主界面信息;
	OBS_PARAM_TYPE_TRAIL_FLOOR,					// 试炼楼层状态;
	OBS_PARAM_TYPE_TOWER_FLOOR,					// 爬塔楼层状态;
	OBS_PARAM_TYPE_TRAIL_BUFF_CONTENT,			// 试炼buff内容;
	OBS_PARAM_TYPE_TRAIL_BUFF_STATE,			// 试炼buff可兑换状态;
	OBS_PARAM_TYPE_TRAIL_REFRESH_TREASURE,		// 刷新宝藏;
	OBS_PARAM_TYPE_TRAIL_OP,					// 获取试炼对手;
	OBS_PARAM_TYPE_TRAIL_OP_STATE,				// 更新试炼对手可选状态;
	OBS_PARAM_TYPE_TRAIL_REWARD,				// 试炼宝箱奖励;
	OBS_PARAM_TYPE_TRAIL_RESET,					// 重置试炼;
	OBS_PARAM_TYPE_TOWER_RESET,					// 重置爬塔;
	OBS_PARAM_TYPE_TRAIL_RANKING,				// 试炼排行榜;
	OBS_PARAM_TYPE_TOWER_RANKING,				// 爬塔排行榜;
	OBS_PARAM_TYPE_TOWER_BUY_CHALLENGE_TIMES,	// 爬塔购买挑战次数;
	OBS_PARAM_TYPE_TRAIL_SCORE,					// 试炼积分;
	OBS_PARAM_TYPE_TOWER_REWARD_SHOW,			// 爬塔奖励预览;
	OBS_PARAM_TYPE_TOWER_SWEEP_COUNT_DOWN,		// 爬塔扫荡倒计时;
	OBS_PARAM_TYPE_TOWER_SWEEP_SETTLEMENT,		// 爬塔扫荡结算;
	OBS_PARAM_TYPE_TOWER_UI_SWEEP_SETTLEMENT	// 爬塔扫荡结算已领取;

};

// 试炼对手;
typedef struct tagTrailOp
{
	int  nType;		// 大于0是玩家，小于0是怪物(此值由服务器给定，本身无意义);
	string strName;
	int  nLv;
	int  nAtk;
	vector<matrixData>  vcFormation;

	tagTrailOp()
		: nType(0)
		, strName("")
		, nLv(0)
		, nAtk(0)
	{
		vcFormation.clear();
	}
}TrailOp;

// 试炼对手节点UI组件;
typedef struct tagUI_Trail_Op_Node
{
	// 标题;
	ImageView*  imgTitle;

	// 卡牌原画;
	Layout*		pnlHeroBg;
	ImageView*  imgHeroBg;
	ImageView*	imgHero;
	ImageView*  imgFrame;

	// 积分;
	Text*  txtScoreTitle;
	Text*  txtScore;

	// 星;
	ImageView*  imgStar;
	Text*  txtStar;
	ImageView*  imgMask2;

	// 玩家信息;
	Text*  txtName;
	Text*  txtLv;
	ImageView*  imgFcDesc;
	Text*  txtFc;

	// 挑战;
	Button*  btnFight;

	// 对手阵型;
	ImageView*  imgMask;
	UIToolHeader*  miniHero[9];

	tagUI_Trail_Op_Node()
	{
		memset(this, 0, sizeof(tagUI_Trail_Op_Node));
	}
}UI_Trail_Op_Node;

// 试炼对手节点UI参数;
typedef struct tagUI_TOWER_OP_PARAM
{
	// 困难级别(1:简单 2:普通 3:困难);
	unsigned int  nLv;

	// 阵型;
	vector<matrixData>  vcOpFormation;

	// 品质;
	vector<int>  vcOpFormationQua;

	// 可获得积分和星数;
	int  nScore;
	int  nStar;

	// 玩家信息;
	int  nType;		// 大于0是玩家，小于0是怪物(此值由服务器给定，本身无意义);
	string  strOpName;
	int  nOpLv;
	int  nOpFc;

	tagUI_TOWER_OP_PARAM()
		: nLv(1)
		, nScore(0)
		, nStar(0)
		, strOpName("")
		, nOpLv(0)
		, nOpFc(0)
	{
		vcOpFormation.clear();
		vcOpFormationQua.clear();
	}
}UI_TOWER_OP_PARAM;

// buff节点UI组件;
typedef struct tagUI_Buff_Node
{
	ImageView*  imgFrame;
	ImageView*  imgBuff;
	Text*  txtName;
	Text*  txtDesc;
	ImageView*  imgStar;
	Text*  txtStarTitle;
	Text*  txtStar;

	tagUI_Buff_Node()
	{
		memset(this, 0, sizeof(tagUI_Buff_Node));
	}
}UI_Buff_Node;

// buff节点UI参数;
typedef struct tagUI_BUFF_PARAM
{
	// 索引(0/1/2);
	int  nIndex;

	// buff类型;
	int  nBuffId;

	// buff名称;
	string  strBuffName;

	// 描述;
	string  strBuffDesc;

	// 价格(消耗星数);
	int  nPrice;

	tagUI_BUFF_PARAM()
		: nIndex(0)
		, nBuffId(0)
		, strBuffName("")
		, strBuffDesc("")
		, nPrice(0)
	{
	}
}UI_BUFF_PARAM;

// 选择对手/星之佑护 公共窗口UI组件;
typedef struct tagUI_Trail_Op
{
	Layout*  pRoot;

	// 窗体背景;
	ImageView*  imgBg;

	// 标题;
	ImageView*  imgTitle1;
	ImageView*  imgTitle2;

	// 关闭按钮;
	Button*	 btnClose;

	// 星;
	ImageView*  imgStarBg;
	Text*  txtStar;

	// Tips;
	Text*  txtTips;

	tagUI_Trail_Op()
	{
		memset(this, 0, sizeof(tagUI_Trail_Op));
	}
}UI_Trail_Op;

typedef struct tagUI_Treasure_Node
{
	UIToolHeader* treasure;
	Text* txtName;
	ImageView*  imgDone;

	tagUI_Treasure_Node()
	{
		memset(this, 0, sizeof(tagUI_Treasure_Node));
	}
}UI_Treasure_Node;

// 发现宝藏窗口UI组件;
typedef struct tagUI_Treasure
{
	Layout*  pRoot;
	Button*  btnClose;
	Button*  btnOpen;
	Text*  txtCost;
	Text*  txtGold;
	Text*  txtLeftRefreshTimes;
	Button*  btnRefresh;
	Text*  txtRefreshCost;
	ImageView*  imgTable;
	ImageView*  imgTreasureBg[6];
	UI_Treasure_Node treasureNode[6];

	tagUI_Treasure()
	{
		memset(this, 0, sizeof(tagUI_Treasure));
	}
}UI_Treasure;


// 试炼主界面信息/战斗记录;
typedef struct tagTrailMainInfo
{
	// 总层数;
	int  nTotalFloor;

	// 当前进行层;
	int  nCurFloor;

	// 历史最高层;
	int  nMaxFloor;

	// 生效的buff列表<typeName, effect>;
	map<string, float>  mapBuffEffect;

	// 积分;
	int  nScore;

	// 星星数量;
	int  nStar;

	// 金币数量;
	int  nGold;

	// 是否到达顶层;
	bool  bEnd;

	// 重置日期;
	//int  nTrailResetDate;

	tagTrailMainInfo()
		: nTotalFloor(1)
		, nCurFloor(1)
		, nMaxFloor(0)
		, nScore(0)
		, nStar(0)
		, nGold(0)
		, bEnd(false)
		//, nTrailResetDate(0)
	{
		mapBuffEffect.clear();
	}
}TrailMainInfo;

// 爬塔主界面信息/战斗记录;
typedef struct tagTowerMainInfo
{
	// 总层数;
	int  nTotalFloor;

	// 当前进行层;
	int  nCurFloor;

	// 历史最高层;
	int  nMaxFloor;

	// 是否正在扫荡中;
	bool bIsSweep;

	// 剩余扫荡时间;
	int  nLeftSec;

	// 是否有扫荡结果未取;
	bool bSweepResult;

	// 剩余可重置次数(不包含VIP);
	int  nLeftResetCount;

	// 是否到达顶层;
	bool  bEnd;

	// 挑战次数;
	//int  nLeftTimes;
	//int  nTotalTimes;

	// 挑战次数购买价格;
	//int  nPrice;

	// 剩余可购买次数;
	//int  nLeftBuyTimes;

	// 重置日期;
	//int  nTowerResetDate;

	tagTowerMainInfo()
		: nTotalFloor(1)
		, nCurFloor(1)
		, nMaxFloor(0)
		, bIsSweep(false)
		, nLeftSec(0)
		, bSweepResult(false)
		, nLeftResetCount(0)
		, bEnd(false)
		//, nLeftTimes(0)
		//, nTotalTimes(0)
		//, nPrice(0)
		//, nLeftBuyTimes(0)
		//, nTowerResetDate(0)
	{
	}
}TowerMainInfo;


// 爬塔UI参数;
typedef struct tagUI_TOWER_PARAM
{
	tagUI_TOWER_PARAM()
	{

	}
}UI_TOWER_PARAM;



// 试炼基础数据(CSV);
typedef struct tagTrailLocalData
{
	// Id;
	int  nBarrierId;

	// 战斗类型;
	TOWER_FIGHT_TYPE  nFightType;

	// 事件类型;
	TRAIL_EVENT_TYPE  nEventType;

	// 通关各难度获得的星数(易/中/难);
	vector<int>  vcStarReward;

	// buff种类以及消耗星数<Id, star>;
	map<int, int>  mapBuff;

	tagTrailLocalData()
		: nBarrierId(-1)
		, nFightType(TFT_FIGHT)
		, nEventType(TET_INVALID)
	{
		mapBuff.clear();
	}
}TrailLocalData;

// 试炼排行榜UI组件;
typedef struct tagUI_TrailRanking
{
	Layout*  pRoot;

	ImageView*  imgRankingBg;
	ImageView*  imgTitleRanking;

	ImageView*  imgRankingDesc;
	Text*  txtRanking;

	ImageView*  imgContentBg;
	cocos2d::ui::ScrollView*  scrollviewContent;
	CustomScrollBar*  scrollBar;

	Button*  btnClose;

	tagUI_TrailRanking()
	{
		memset(this, 0, sizeof(tagUI_TrailRanking));
	}
}UI_TrailRanking;

// 试炼排行榜UI参数;
typedef struct tagUI_TRAIL_RANKING_PARAM
{
	// 我的排名;
	int  nRank;

	// 我的历史最高层;
	int  nMaxFloor;

	// 排行数据;
	vector<RankingData>  vcRankingData;

	tagUI_TRAIL_RANKING_PARAM()
		: nRank(0)
		, nMaxFloor(1)
	{
	}
}UI_TRAIL_RANKING_PARAM;


///////////////////////////////////////////////////////////////////

// 爬塔主界面UI组件集;
typedef struct tagUI_Tower_Main
{
	Layout*		pRoot;

	// 入口;
	Layout*		pnlMain;
	ImageView*	imgMainBg;
	ImageView*	imgTowerTitle;
	Button*		btnEnterTower;
	ImageView*	imgTrailTitle;
	Button*		btnEnterTrail;
	Button*		btnHelp;
	ImageView*  imgTrailEnterLv;

	tagUI_Tower_Main()
	{
		memset(this, 0, sizeof(tagUI_Tower_Main));
	}
}UI_Tower_Main;

// 爬塔界面UI组件;
typedef struct tagUI_Tower
{
	Layout* pRoot;

	// 背景;
	ImageView*	imgTowerBg;

	// 爬塔;
	Layout*		pnlTower;

	// 排行榜;
	Button*		btnRanking;

	// 进度;
	ImageView*  imgProgressBg;
	ImageView*  imgCursor;
	Text*		txtProgressNode[5];

	// 奖励预览;
	Button*		btnReward;
	Button*		btnBox;
	//ImageView*  imgArrow;
	Layout*		pnlReward;
	ImageView*  imgRewardBg;
	ImageView*	imgReward;
	Text*		txtExp;
	Text*		txtCoin;

	// 重置;
	Button*		btnReset;

	// 扫荡;
	Button*		btnSweep;
	Text*		txtTime1;
	Text*		txtTime2;
	Layout*		pnlSweep;
	Button*		btnSweepOK;
	Button*		btnSweepCancel;
	Text*		txt1;
	Text*		txt2;
	Text*		txtFloor;
	Text*		txtSweepTime;

	// 扫荡结果;
	Layout*		pnlSweepSettlement;
	Button*		btnSettlementClose;
	ImageView*	imgContentBg;
	cocos2d::ui::ScrollView* scrollviewContent;
	Text*		txtSettlementCoin;
	Text*		txtSettlementExp;
	CustomScrollBar* scrollBar;

	// 历史最高层数;
	Text*		txtMaxFloor;

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

	tagUI_Tower()
	{
		memset(this, 0, sizeof(tagUI_Tower));
	}
}UI_Tower;

// 试炼界面UI组件;
typedef struct tagUI_Trail
{
	Layout* pRoot;

	// 背景;
	ImageView*	imgTrailBg;

	// 试炼;
	Layout*		pnlTrail;
	Button*		btnTrailReset;
	Button*		btnStore;

	// 进度;
	ImageView*  imgProgressBg;
	ImageView*  imgCursor;
	Text*		txtProgressNode[5];

	//Text*		txtTrailGold;
	Text*		txtTrailScore;
	Text*		txtTrailStar;

	//ImageView*  imgBuffBg;
	//Layout*		pnlBuffTips;
	//Text*		txtBuffName;
	//Text*		txtBuffDesc;
	Button*		btnBuff;
	ImageView*  imgArrow;
	ImageView*  imgBufTips;
	Text*		txtBuffNone;

	tagUI_Trail()
	{
		memset(this, 0, sizeof(tagUI_Trail));
	}
}UI_Trail;


typedef struct tagUI_Reward_Preview
{
	Layout*			pRoot;
	ImageView*		imgPreviewBg;
	Button*			btnClose;
	ImageView*		imgBg;
	cocos2d::ui::ScrollView*		scrollviewContent;
	CustomScrollBar* scrollBar;

	tagUI_Reward_Preview()
	{
		memset(this, 0, sizeof(tagUI_Reward_Preview));
	}
}UI_Reward_Preview;

typedef struct tagRewardPreviewParam
{
	// 第几层;
	int nIndex;

	// 奖励内容<Id, Count>;
	map<int, int>  mapReward;

	tagRewardPreviewParam()
		: nIndex(0)
	{
		mapReward.clear();
	}
}RewardPreviewParam;

typedef struct tagTowerSweepSettlement
{
	int nExp;
	int nCoin;

	// 奖励物品<Id, Count>;
	map<int, int>  mapItem;

	size_t size()
	{
		return mapItem.size();
	}

	bool empty()
	{
		return (nCoin == 0 && nExp == 0 && mapItem.size() == 0);
	}

	// 清理;
	void clear()
	{
		nExp = 0;
		nCoin = 0;
		mapItem.clear();
	}

	tagTowerSweepSettlement()
	{
		clear();
	}
}TowerSweepSettlement;



#endif // TowerDef_h__
