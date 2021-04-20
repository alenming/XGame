#ifndef BossDataDef_h__
#define BossDataDef_h__

#include <iostream>
using namespace std;

//刷新冷却时间和鼓舞消耗
#define CLEAR_COOL_TIME_COST  10096
#define	INSPIRE_COST		  10097

//boss奖励起始/终止ID(表格配置)
#define BOSS_REWARD_ID_BEGIN	16	
#define BOSS_REWARD_ID_END		27

typedef struct bossInfoFromSvr
{
	int bossId;		
	int bossHp;	
	int rewardCoin;
	int attribute;

	//挑战需要的等级
	int needLv;
	
	//击杀状态
	int state;

	string bossName;
	string killer;
}sBossInfoFromSvr;

typedef struct damageRank
{
	//名次
	int rankId;

	//伤害
	string damage;

	//名称
	string name;
}sDamageRank;

//上轮战绩
typedef struct lastDamRank
{
	int resID;
	string name;
	int level;
	int damage;
	int rank;
	string totalDam;
}sLastDamRank;

//奖励信息
typedef struct rewardIDInfo
{
	int rewardID;
	
	//是否可领取/不可领取则表示已领取过
	bool isCanGetted;			
}sRewardIDInfo;

#endif