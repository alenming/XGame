#ifndef PvpHeader_h__
#define PvpHeader_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "Communication/Socket/Util.h"
#include "HeroModule/HeroModel.h"
#include "MainModule/MainModel.h"
#include "Common/Common.h"
#include "FormationModule/CustomScrollBar.h"
#include "GuideCenter/GuideManager.h"
#include "ChapterModule/SceneChapterLoading.h"
#include "2d/CCActionInterval.h"
#include "MainModule/MainCityScene.h"
#include "Utils/ResourceUtils.h"
#include "Temp/PopReward.h"
#include "FormationModule/LayerFormation.h"
#include "FightModule/SceneFightLoading.h"
#include "FightModule/FightController.h"
#include "HeroModule/HeroModel.h"
#include "CommonBuy/LayerCommonBuy.h"
#include "Shopexc/LayerShopexc.h"
#include "Widget/LayerCommHeadInfo.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

const int TEAMMATE_COUNT = 9;
const int PVP_FREE_COUNT = 10128;
const int PVP_BUY_COUNT_SPEND = 10127;

enum PVP_EVENT_TYPE
{
	PVP_EVENT_TYPE_GET_PVP_INFO,
	PVP_EVENT_TYPE_LAUCH_PVP_FIGHT,
	PVP_EVENT_TYPE_SETMENT_ACCOUNT,
	PVP_EVENT_TYPE_RANK,
	PVP_EVENT_TYPE_WIN_PRIZE,
	PVP_EVENT_TYPE_BUY_COUNT,
};

enum PVP_RANK_TYPE
{
	PVP_RANK_TYPE_PVP = 1, //竞技场
	PVP_RANK_TYPE_WIN, //胜场排行
};

struct matrixData
{
	int id;
	int stars;

	matrixData()
	{
		id = 0;
		stars = 0;
	}
};

struct PVP_PLAYER_DATA
{
	int      resID;  //资源id
	string   name;   //名字
	string   groupName;	//帮派名
	int      level;  //等级
	int      atk;    //战斗力
	int      pvporder; //pvp排名
	int		 vipLv;
	vector<matrixData> matrixIds; //阵容
	int     id; //玩家id

	Widget *layer;

	PVP_PLAYER_DATA()
		: resID(0), level(0), atk(0), pvporder(0),
		vipLv(0), id(0), name(""), groupName("") {}
};
typedef vector<PVP_PLAYER_DATA> PVP_PLAYER_DATA_VEC;

//PVP主界面控件
struct  UI_PVP_ENTER
{
	// UI根节点层容器
	Layout		*pRoot;

	ImageView   *pvpPlayer;   //挑战的玩家
	Layout		*panelList;
	Layout		*panelListNode;
	ImageView   *playerListDi;
	Layout      *tableView;  //挑战玩家列表
	Text        *PvpCost;  //精力消耗
	Button      *pBtnhelp;    //显示帮助
	Button      *pBtAddPvpvCount;//增加pvp次数

	Layout      *heroInfoPanle; //角色信息
	ImageView   *imgNamePanel; 
	ImageView   *rankImgOder;
	Text        *rankOrder;   //排名
	Text        *mineName;    //名字
	Layout      *mineHero;    //英雄
	Text        *mineFight;   //战斗力
	ImageView   *Image_force_di;
	ImageView   *Image_force_1;
	Text        *Label_force_2;
	ImageView   *Image_reward_di;

	Text        *texTodayRank; //
	Text        *todayHonorCount;   //
	Button      *pBtnRankPrize;
	Button      *pBtnAdjustment; //调整整形
	Button      *pBtnWinPrize; //胜场奖励
	Button      *pBtnHonorShop;//荣誉商店
	Button      *pBtnRankList;//排行榜

	Layout      *Panel_RankReward;  //排行榜奖励
	ImageView*RankReward;  //排行榜奖励
	ListView    *RankPrizeList;  //排行榜奖励列表
	Button      *pBtnRankRewardClose;    //显示帮助

	Layout      *fightPanel;  //战斗
	Layout      *fightDesc;//战斗总结
	ImageView   *imgCurRank;//当前排名图片
	ImageView   *imgHistoryBestRank;//历史最高排名图片
	Text        *curRank;//当前名次
	Text        *bestRank;//最高名次
	ImageView   *curUp;//当前箭头
	ImageView   *bestUp;//最高箭头
	Text        *curUpCount;//当前上升名次
	Text        *bestUpCount;//最高上升名次
	Text        *getPrizeGold;//获得金币
	Text        *getPrizeTips;//提示

	Layout      *Panel_VictoryReward;
	ImageView   *VictoryReward;  //胜场奖励
	ListView    *VictoryRewardList;//胜场奖励列表
	Button      *pBtnVictoryRewardClose; //胜场列表关闭
	Text        *VictoryRewardCount; //胜场次数描述

	Layout      *Panel_Ranking;
	ImageView   *Ranking; //排行榜
	ListView    *RankingList; //排行榜列表
	Button      *pBtnRankingListClose; //关闭
	Text        *RankingCount;//我的排名
	Text        *RankingBestCount;//历史最高我的排名
};

struct PVP_REWARD_NODE
{
	int templateId;
	int amount;
};

struct PVP_REWARD_ITEM
{
	int top;
	int bot;
	vector<PVP_REWARD_NODE> prize;
};

struct PVP_SignRewardInfo
{

	//道具ID
	int mItemId;

	//道具数量
	int mItemCount;
	
};

//弹出获得的奖励
struct PVP_ReWard_Show
{
	vector<PVP_SignRewardInfo> mVecRewardInfo;
	int mGold;
	int mCoin;
};

typedef vector<PVP_REWARD_ITEM> PVP_REWARD_ITEM_VEC;

struct PVP_SHOP_NODE_INFO
{
	int costRank;
	int price;
	int amount;
	int id;
};

enum  PVP_DAY_WIN_TYPE
{
	PVP_DAY_WIN_TYPE_NONE = 0, //不可领
	PVP_DAY_WIN_TYPE_GET, //可领
	PVP_DAY_WIN_TYPE_GETED //已领
};
typedef vector<PVP_SHOP_NODE_INFO> PVP_SHOP_NODE_INFO_VEC;

#endif
