#ifndef FirstRechargeModel_h__
#define FirstRechargeModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"

typedef struct Reward
{
	int id;
	int count;

	Reward()
	{
		id = 0;
		count = 0;
	}
}sReward;

class FirstRechargeModel : public Observable
{
public:

	static FirstRechargeModel* getInstance();
	static void destroyInstance();

	//获取首充奖励列表
	vector<sReward>& getFirstRechargeReward();

private:
	FirstRechargeModel();
	virtual ~FirstRechargeModel();

	//解析首充奖励列表
	void initDataFromLocal();

public:

	//首充状态 0-首充还在   1-首充完成但未领取奖励  2-首充奖励已领取
	int mFirstRechargeState;

private:

	static FirstRechargeModel* ms_pInstance;

	//奖励列表
	vector<sReward> m_vRewardList;
};


#endif