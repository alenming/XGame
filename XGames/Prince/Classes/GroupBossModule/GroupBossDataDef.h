/******************帮派boss数据定义**********************/

#ifndef GroupBossDataDef_h__
#define GroupBossDataDef_h__

//boss最大生命数量
#define BOSS_MAX_LIFE		4
//表格buff鼓舞ID
#define INSPIRE_COST_DATA_ID	10133
#define INSPIRE_BUFF_ADD_ID		10136
#define INSPIRE_MAX_TIMES_ID	10137

//金币银币ID
#define GOLD_SYSTEM_ID			11001
#define COIN_SYSTEM_ID			11002

//boss宝箱奖励起始ID
#define BOSS_REWARD_START_ID	3001

//boss战斗场景资源ID
#define BOSS_FIGHT_SCENE_RESID	10135

//锦囊ID
#define JIN_NANG_TOOL_ID		10071

//全服，帮派内部排名
typedef enum rankType 
{
	eTYPE_GROUP = 1,		//帮派排名
	eTYPE_ALL,				//全服排名
	eTYPE_IN_GROUP			//帮派内部排名
}eRankType;

typedef enum getType 
{
	eTYPE_HURT = 1,			//伤害奖励
	eTYPE_JINNANG			//锦囊兑换
}eGetType;

//宝箱奖励类型(对应配表字段)
typedef enum rewardType
{
	eTYPE_DAM_EXCHANGE = 2,	//伤害兑换
	eTYPE_KILLED_BOSS = 3	//击杀boss的宝箱
}eRewardType;

//领取宝箱方式
typedef enum getRewardMethod
{
	eMETHOD_ALL,			//一键领取
	eMETHOD_SINGLE			//领取指定ID的宝箱
}eGetRewardMethod;

//boss相关数据和状态
typedef struct groupBossInfo
{
	int bossOrder;			//boss序列号
	int totalBossNum;		//boss总序列数
	int bossId;
	int bossIdx;			//战斗结算会用到
	int bossResId;
	int bossAttri;			//boss水木火属性
	int bossLv;
	int restReviveTimes;	//boss剩余复活次数
	int totalReviveTimes;	//boss总共几条命
	int reviveCoolTime;		//boss复活冷却时间
	time_t curHp;
	time_t totalHp;

	int bossNickResId;		//boss昵称资源
	string bossName;		//boss名称

	groupBossInfo()
		: bossOrder(0), bossId(-1), bossResId(-1), bossAttri(-1)
		, bossLv(0), restReviveTimes(0), totalReviveTimes(0)
		, reviveCoolTime(0), curHp(0), totalHp(0), bossIdx(0)
		, totalBossNum(0), bossNickResId(0), bossName("") {}
}sGroupBossInfo;


//排名相关数据结构
//1. 各帮派伤害排名
typedef struct groupDamRank
{
	int rank;
	int flagResId;			//帮派旗帜id
	int groupLv;
	int killedBossNum;		//击杀boss数量
	string groupName;
	string damageVal;
}sGroupDamRank;

//2. 全服玩家/帮派成员伤害排名
typedef struct playerDamRank
{
	int rank;
	int headerResId;		//头像ID
	int vipLv;
	int level;
	string playerName;
	string damageVal;
}sPlayerDamRank;

//boss宝箱奖励数据
typedef struct rewardItemData
{
	int rewardId;
	int rewardCnt;
}sRewardItemData;

typedef struct boxReward
{
	int boxId;
	string bossName;		//每个boss对应有自己的一组宝箱奖励
	bool isGetted;			//是否领取过
	bool isCanGet;			//是否可领取
	vector<sRewardItemData> vBoxRewardInfo;

	boxReward()
		: boxId(-1), bossName(""), isGetted(false), isCanGet(false)
	{ 
		vBoxRewardInfo.clear(); 
	}
}sBoxReward;

//排名宝箱奖励
typedef struct DamReward
{
	int boxId;
	time_t damLimit;		//伤害阈值
	bool isGetted;
	bool isCanGet;
	vector<sRewardItemData> vDamRewardInfo;
	DamReward(): boxId(0), damLimit(0), isGetted(0), isCanGet(0)
	{ 
		vDamRewardInfo.clear();
	}
}sDamReward;

typedef struct ExchangeJn
{
	int boxId;			//头Id
	int firstId;		//锦囊ID
	int secondId;		//兑换物ID
	int firstCount;		//锦囊个数
	int secondCount;	//兑换个数
	std::string name;		//兑换物介绍
	ExchangeJn(): boxId(0), firstId(0), secondId(0), firstCount(0), secondCount(0), name("")
	{ 
	}
}sExchangeJn;

#endif