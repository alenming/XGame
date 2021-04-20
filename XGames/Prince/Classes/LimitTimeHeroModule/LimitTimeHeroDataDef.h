#ifndef LimitTimeHeroDataDef_h__
#define LimitTimeHeroDataDef_h__

#include "cocos2d.h"
#include "iostream"

using namespace std;
using namespace cocos2d;

//返回的抽到的道具数量
const int iItemNumOne = 1;
const int iItemNumTen = 10;

//活动图标显示
#define LIMIT_TIME_HERO_ACT_SHOW_NO			0
#define LIMIT_TIME_HERO_ACT_SHOW_YES		1
#define LIMIT_TIME_HERO_ACT_UPDATE_TIME		2

//活动形式
typedef enum ActType
{
	eMETHOD_TAKECARD = 1,				//抽卡
	eMETHOD_OPENBOX = 2					//抽宝匣
}eActType;

//活动开启的时间类型
typedef enum ActOpenTimeType
{
	eTIME_TYPE_OPENSVR = 1,				//开服
	eTIME_TYPE_LIMIT = 2				//限时
}eActOpenTimeType;

//抽卡方式
typedef enum ChouKaType
{
	eTYPE_ONE,
	eTYPE_TEN
}eChouKaType;

//时间戳类型
typedef enum ActTimeStampType
{
	eTAMP_TYPE_OPEN,
	eTAMP_TYPE_OVER,
	eTAMP_TYPE_SHOW,
	eTAMP_TYPE_FREE
}sActTimeStampType;

//活动开启状态
typedef enum ActState
{
	eSTATE_WAIT,					//等待开启
	eSTATE_OPEN,					//开启在进行中
	eSTATE_SHOW,					//活动不能进行但需要展示
	eSTATE_OVER						//活动已结束
}eActState;

//活动时间
typedef struct ActTime
{
	int		svrOpenTime;			//开服时间
	int		actOverSec;				//活动结束(展示之前)剩余秒数
	time_t	actOpenTime;			//活动开启时间
	time_t	actOverTime;			//活动结束时间(展示之前)
	time_t	actShowTime;			//活动展示时间
	eActOpenTimeType timeType;		//时间类型

}sActTime;

//x天、xx时xx分xx秒
typedef struct FormatTime
{
	string strDays;
	string strHMS;		//时分秒
}sFormatTime;

//奖励数据
typedef struct RankRewardItem
{
	int rewardId;
	int rewardCnt;
}sRankRewardItem;

//积分奖励盒子
typedef struct RewardBox
{
	int boxId;				//下标
	bool isCanGet;			
	bool isGetted;
	int scorePoint;			//积分点
	std::vector<sRankRewardItem> vecRewardData;
}sRewardBox;

//活动奖励排名段
typedef struct RankSection
{
	int beginRank;
	int endRank;
}sRankSection;

//活动排名奖励数据
typedef struct RankRewardData
{
	int id;
	int actId;
	sRankSection rankSection;
	string rewardTips;
	std::vector<sRankRewardItem> rankRewardItem;
	int limit;
}sRankRewardData;

//活动数据
typedef struct ActData
{
	int actId;
	eActState actState;
	string actName;
	eActType actType;
	sActTime actTime;
	std::vector<sRewardBox> vecBoxRewardData;
	std::vector<sRankRewardData> vecRankRewardData;

	//限时神将ID
	int heroId;			
}sActData;

//积分排名数据
typedef struct ScoreRank
{
	int rank;
	string name;
	int score;
}sScoreRank;

// 抽到物品数据;
typedef struct GetItemParam
{
	int			nId;			// Id;
	string		strName;		// 名称;
	int			nCount;			// 数量;
	int			countChips;		//shui
	GetItemParam()
		: nId(-1)
		, strName("")
		, nCount(0)
		, countChips(0) {}
}sGetItemParam;

#endif