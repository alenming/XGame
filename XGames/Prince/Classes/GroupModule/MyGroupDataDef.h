/******************定义一些我的帮派数据结构*******************/

#ifndef MyGroupDataDef_h__
#define MyGroupDataDef_h__

#include "PvpModule/PvpHeader.h"

//职位权限
typedef enum postPower
{
	ePOWERDEFAULT,									
	ePOWERLEADER,									//帮主权限--1
	ePOWERVICE,										//副帮主权限--2
	ePOWERNOPOWER									//帮众权限--3 
}ePostPower;

//帮派信息结构
typedef struct MyGroupData
{
	int id;											//帮派ID								
	int resId;										//帮派头像ID
	int level;										//帮派等级
	int todayExp;									//今日捐赠值
	int todayMaxExp;								//今日捐赠最大值
	int totalMemberNum;								//帮派容量
	int curMemberNum;								//帮派当前人数
	int curViceNum;									//当前副帮主数量
	int rank;										//帮派排名
	string name;									//帮派名称
	string leaderName;								//帮主名
	string viceName1;								//副帮主名
	string viceName2;								//副帮主名

	MyGroupData()
		: id(0), resId(0), level(0), todayExp(0), todayMaxExp(0)
		, totalMemberNum(0), curMemberNum(0), curViceNum(0), rank(0)
		, name(""), leaderName(""), viceName1(""), viceName2("")
	{

	}
}sMyGroupData;


//帮众成员信息
typedef struct PlayerInfo
{
	int uId;										//玩家ID
	int	iconResId;									//玩家头像ID
	int level;										//玩家等级
	int vipLevel;									//玩家vip等级
	int contributeVal;								//玩家贡献值
	int fightPower;									//玩家战斗力
	bool isOnLine;									//是否在线
	string name;									//玩家名称
	time_t	lastLoginTime;							//玩家最后登录时间
	ePostPower power;								//玩家职位权限
	vector<matrixData>	matrix;						//玩家阵型

	PlayerInfo()
		: uId(0), level(0), vipLevel(0), contributeVal(0)
		, iconResId(0), fightPower(0), isOnLine(false)
		, name(""), lastLoginTime(0), power(ePOWERDEFAULT) {}
}sPlayerInfo;


//我的信息
typedef struct MyInfo
{
	int myPrestige;									//我的声望
	int myMaxDonateTimes;							//我的每天最多捐献次数
	int myCurDonateTimes;							//我的捐献次数

	sPlayerInfo info;

	MyInfo()
		: myPrestige(0), myMaxDonateTimes(0), myCurDonateTimes(0) 
	{}
}sMyInfo;

//申请人信息
typedef struct ProposerInfo
{
	int uId;
	int level;	
	int iconResId;									//头像资源Id
	int vipLv;										//vip等级
	int fightPower;									//战斗力
	string name;

	ProposerInfo()
		: uId(0), level(0), iconResId(0)
		, vipLv(0), fightPower(0)
		, name("") {}

}sProposerInfo;

//帮派宣言
//宣言类型
typedef enum DeclarationType
{
	eDECLARATION_DEFAULT_TYPE,
	eDECLARATION_INSIDE_TYPE,						//对内宣言
	eDECLARATION_OUTSIDE_TYPE						//对外宣言
}eDeclarationType;

//宣言内容
typedef struct Declaration
{
	string insideDec;								//对内宣言
	string outsideDec;								//对外宣言

	Declaration()
		: insideDec(""), outsideDec("")
	{

	}

}sDeclaration;

//帮派动态事件
//事件日期
typedef struct EventDate
{
	int year;
	int month;
	int day;

	EventDate()
		: year(0), month(0), day(0) {}
}sEventDate;

typedef struct DynamicEvent
{
	sEventDate eventDate;							//事件日期
	string eventTime;								//事件具体时间	xx:xx 时分
	string eventContent;							//事件内容

	DynamicEvent()
		: eventTime(""), eventContent("")
	{

	}

}sDynamicEvent;

/**********帮派建筑相关**********/
//帮派建筑类型
typedef enum BuildType
{
	eBUILD_DEFAULT_TYPE,
	eBUILD_JUYITING_TYPE,							//聚义厅
	eBUILD_DOSNATE_SHOP_TYPE,						//声望商店
	eBUILD_DOSNATE_XUNLONG							//寻龙阁

}eBuildType;

typedef struct BuildInfo
{
	int id;											//建筑ID
	int resId;										//建筑图标ID
	int	curExp;										//建筑当前经验
	int nextExp;									//建筑下一级经验
	int level;										//建筑等级
	int reset;										//寻龙阁可刷新次数
	string name;									//建筑名称
	string curEffect;								//建筑当前效果
	string nextEffect;								//建筑下一级效果
	eBuildType type;								//建筑类型

	BuildInfo()
		: id(0), resId(0), curExp(0), nextExp(0), level(0), reset(0)
		, name(""), curEffect(""), nextEffect("")
		, type(eBUILD_DEFAULT_TYPE) {}

}sBuildInfo;

//捐赠类型
typedef enum DonateType
{
	eDONATE_DEFAULT_TYPE,
	eDONATE_GENERAL_TYPE,							//普通捐献
	eDONATE_ADVANCE_TYPE,							//高级捐献
	eDONATE_RICH_TYPE								//土豪捐献
}eDonateType;

//捐献表格数据
typedef struct DonateLocalDT
{
	int id;
	string name;
	int costCoin;									//花费的银币
	int costGold;									//花费的金币
	int addExp;										
	int prestige;									//声望
	int contriVal;									//贡献

	DonateLocalDT()
		: id(0), name(""), costCoin(0), costGold(0)
		, addExp(0), prestige(0), contriVal(0) {}

}sDonateLocalDT;

//帮派申请条件
typedef enum JoinType
{
	eJOIN_DEFAULT_TYPE,	
	eJOIN_CHECK_TYPE,								//审核加入
	eJOIN_FREE_TYPE									//自有加入

}eJoinType;

typedef struct JoinLimit
{
	int levelLimit;								//等级需求							
	eJoinType type;							

	JoinLimit()
		: levelLimit(0), type(eJOIN_DEFAULT_TYPE) {}

}sJoinLimit;

//离开帮派的事件
typedef enum ExitGroupEvent
{
	eEXIT_DEFAULT_EVENT,
	eEXIT_LEAVE_EVENT,								//主动离开了帮派
	eEXIT_DISMISS_EVENT,							//解散了帮派
	eEXIT_EXPEL_EVENT,								//被逐出了帮派
	eEXIT_DISBANDED_EVENT							//被解散了帮派				

}eExitGroupEvent;

//活动类型
typedef enum ActivityType
{
	ActivityType_None,								//
	ActivityType_XunLong,							//寻龙
	ActivityType_Other
}eActivityType;

typedef struct NineList
{
	int resId;								//道具ID							
	int count;								//道具数量
	bool isCanClick;						//是否已经点击过
	int prince;								//价格
	int flag;								//下标

	NineList(): resId(0), count(0),isCanClick(false),prince(0),flag(0) {}

}sNineList;

//寻龙得到的物品
typedef struct ActivityFind
{
	int id;
	int count;
	ActivityFind():id(0),count(0)
	{}
}sActivityFind;

//活动数据
typedef struct ActivityInfo
{
	std::vector<vector<int>> nineList; //9个没开始寻龙之前展示道具

	std::vector<std::vector<sNineList>> clickedHistory; //上一次点击的历史

	//展示物品
	std::vector<int> showItem;
	int showHero;

	//有过点击之后的下标
	std::vector<int> showedIdflag;

	int importantId;			//重要ID

//	bool isFree;

	int goldCost;					//当前抽的价格

	int clickCount;					//本轮已经点击的次数

	int searchID;					//展示物品的查询ID

	int isXunlong;					//0为没点过寻龙按钮,1为点过了

	sActivityFind actFind;			//寻龙得到的物品

	ActivityInfo()
		: showHero(0)
		, importantId(0)
		, goldCost(0)
		, clickCount(0)
		, isXunlong(0)
		, searchID(0)
		, nineList(0)
		, showedIdflag(0)
		, showItem(0)
		, clickedHistory(0)
	{}

}sActivityInfo;

typedef struct ActivityBuildInfo
{
	int id;											//建筑ID
	int resId;										//建筑图标ID
	string name;									//建筑名称
	eActivityType type;								//活动类型
	int _sort;										//顺序
	std::vector<int> showId;						//展示的物品
	string tips;									//建筑当前效果


	ActivityBuildInfo():
		id(0)
		, resId(0)
		, _sort(0)
		, name("")
		, tips("") 
		, type(ActivityType_None){}

}sActivityBuildInfo;

#endif	//MyGroupDataDef_h__