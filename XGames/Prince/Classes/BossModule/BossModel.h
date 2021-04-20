#ifndef BossModel_h__
#define BossModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "BossDataDef.h"

class BossModel : public Observable
{
public:
	static BossModel* getInstance();
	static void destroyInstance();

	//根据服务器返回的伤害计算奖励ID
	void setRewardId();

	//领取奖励后重置奖励列表ID
	void resetRewardId();

	//解析战斗完切回到boss场景消息
	void parseEndLastFightToScene(const Json::Value& data);

private:
	BossModel();
	virtual ~BossModel();

public:
	int m_bIsOpen;							//活动是否开启
	int m_iSeconds;							//开启倒计时、活动剩余时间
	sDamageRank m_sTop[3];					//前三名
	int m_iFightCoolSeconds;				//战斗冷却时间
	vector<sBossInfoFromSvr> m_vBossInfoFromSvr;
	float m_iDamageAdd;						//伤害加成
	int m_iInspireTimes;					//鼓舞次数
	int m_iAtkBossId;						//当前攻击的bossid，方便战斗模块调用
	int m_iIsKiller;						//是否我击杀

	string m_myTotalDamage;					//我的伤害
	int m_iMyRank;							//我的名次

	string m_myLastTotalDam;				//我上一轮的伤害
	int m_iMyLastRank;						//我上一轮的排名

	int m_iTotalDamage;						//我的伤害(服务下发未转格式，用来计算奖励ID)

	vector<sLastDamRank> m_vLastRankList;	//上轮战绩列表
	vector<int> m_vRewardIDList;			//已经领取过的奖励列表
	vector<sRewardIDInfo> m_vRewardIDInfo;	//奖励ID信息(已领取和未领取)

	bool m_bIsFightSettement;				//是否战斗结算重拉boss模块主界面
	int m_iCurSeledBoss;					//当前选中的boss序列号
	int m_iIsEndActive;						//活动是否结束

	int m_iCurInspireCost;					//本次鼓舞花费
	int m_iCurClearCoolCost;				//本次消除冷却花费
private:

	static BossModel* ms_pInstance;
};

#endif