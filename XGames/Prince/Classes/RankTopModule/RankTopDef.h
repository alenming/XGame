#ifndef RankTopDef_h__
#define RankTopDef_h__

#include "PvpModule/PvpHeader.h"

const int MAX_RANK_NUM	= 4;

enum rankTopType
{
	DEFAULT_RANK_TOP,
	LEVEL_RANK_TOP,		//等级排行榜
	POWER_RANK_TOP,		//战斗力排行榜
	PVP_RANK_TOP,		//竞技排行榜
	GROUP_RANK_TOP		//帮派排行榜
};

typedef struct rankTopInfo
{
	rankTopType rankType;			//类型
	int	rank;						//名次		
	int iconId;						//头像ID	(帮派时为帮派资源ID)
	string name;					//玩家昵称	(帮派时为帮主名称)
	string group;					//帮派名称	
	int level;						//等级		(帮派时为帮派等级)
	int power;						//战力		(帮派时为帮派总战斗力)
	int uId;						//玩家ID	(帮派时为帮派ID)
	int vipLv;

	vector<matrixData>	matrix;		//玩家阵型	(帮派时为空)

	int curNum;						//帮派当前成员数
	int maxNum;						//帮派总成员数
	string notice;					//帮派公告

	rankTopInfo()
		:rankType(DEFAULT_RANK_TOP)
		,rank(0)
		,iconId(0)
		,name("")
		,group("")
		,level(0)
		,power(0)
		,uId(0) 
		,curNum(0)
		,maxNum(0)
		,vipLv(0)
		,notice("") { matrix.clear(); };
}sRankTopInfo;

typedef struct myRankTopInfo
{
	int myLevel;
	int myLevelRank;	
	int myPower;
	int myPowerRank;
	int myPvpRank;
	int myGroupRank;
	int myGroupPower;
	
	myRankTopInfo()
		:myLevel(0)
		,myLevelRank(0)
		,myPower(0)
		,myPowerRank(0)
		,myPvpRank(0)
		,myGroupRank(0)
		,myGroupPower(0) {};
}sMyRankTopInfo;

#endif	RankTopDef_h__