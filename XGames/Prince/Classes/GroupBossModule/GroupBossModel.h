
/******************帮派boss数据处理**********************/

#ifndef GroupBossModel_h__
#define GroupBossModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "GroupBossDataDef.h"
#include "Utils/LocalTimer.h"


class GroupBossModel : public Observable, public TimeStampListener
{
public:

	static GroupBossModel* getInstance();
	static void destroyInstance();

	//本地数据初始化
	void initFromLocal();

	//服务端数据初始化
	void initFromSvr(const Json::Value& data);

	//更新排名数据
	void updateRankData(const Json::Value& data);

	//更新鼓舞信息
	void updateInspireInfo(const Json::Value& data);

	//单次领取奖励
	void updateGetRewardInfo(const Json::Value& data);

	//单次锦囊兑换信息
	void updateExchangeJnang(const Json::Value& data);

	//解析主界面拉取的BOSS伤害可领项
	void updateBossCanGetInfo(const Json::Value& data);
	//排序
	void sortDamRewardData();

	//解析主界面拉取的兑换选项
	void updateExchangeCanInfo(const Json::Value& data);

	//更新击退boss宝箱奖励数据
	void updateKilledBossRewardData(const Json::Value& data);

	//是否需要更新冷却倒计时
	bool getIsNeedUpdateCoolTime();

	//更新到UI
	void notifyUiLayer(int type, void* data = nullptr );

public:
	//记录一些发送协议时的中间变量
	CC_SYNTHESIZE(eRankType, m_rankTemp, RankTemp);
	CC_SYNTHESIZE(int, m_rewardId, RewardId);
	CC_SYNTHESIZE(int, m_restLife, RestLife);

public:
	//获取指定ID的伤害奖励宝箱
	sDamReward* getDamRewardByID(int boxId);

	//获取所有伤害奖励宝箱
	std::vector<sDamReward>* getDamReward();
	
	//是否可兑换指定ID奖励
	bool isCanExchangeByID(int boxId);
	//兑换数据
	vector<sExchangeJn>* getExchangeJn();
	//获取指定ID的奖励数据
	sExchangeJn* getExchangeJnByID(int boxId);

	//获取指定ID的boss奖励宝箱
	sBoxReward* getBoxRewardByID(int boxId);

	//获取宝箱信息
	vector<sBoxReward>* getBoxReward();

	//获取帮派伤害排名数据(有序)
	vector<sGroupDamRank>* getGroupRankData();

	//获取我的帮派排名数据
	sGroupDamRank* getMyGroupRankData();

	//获取个人伤害排名数据(有序)
	vector<sPlayerDamRank>* getPlayerRankData(eRankType type);

	//获取我的伤害排名数据
	sPlayerDamRank* getMyRankData(eRankType type);

	//获取主界面排行榜数据
	vector<sGroupDamRank>* getMainGroupRankData();
	vector<sPlayerDamRank>* getMainPlayerRankData();

	//获取剩余挑战次数
	int getRestAtkCount();

	//获取鼓舞次数
	int getInspireCount();

	//获取我的累计伤害
	time_t getMyTotalDam();

	//获取剩余时间
	int& getRestTime();

	//获取boss信息
	sGroupBossInfo* getBossInfo();

	//是否有boss击退宝箱可以领取
	bool isCanGetBossReward();

	//是否有伤害奖励领取 或者 有锦囊可以兑换
	bool isCanGetDamReward();
	//是否可以进行兑换
	bool isCanExchange();
	

	//活动是否开启
	bool isActOpen();

	//boss是否可攻击
	bool isBossCanAtk();

	//下次鼓舞需要的金币
	int getNextCost();

	//获取最大鼓舞次数
	int getMaxInspireCount();

	//获取当前展示的boss宝箱对应的bossID
	int getCurRewardBossId();

	//获取伤害加成
	float getDamAdded();

	//是否需要更新冷却倒计时
	void setIsNeedUpdateCoolTime(bool isNeed);

private:
	GroupBossModel();
	virtual ~GroupBossModel();

	//设置活动开启状态
	void setActState(bool isActOpen);

	//主界面排名数据
	void updateMainGroupRankData(const Json::Value& data);
	void updateMainPlayerRankData(const Json::Value& data, eRankType type);


	//更新帮派排名数据
	void updateGroupRankData(const Json::Value& data);

	//更新我的帮派排名数据
	void updateMyGroupRankData(const Json::Value& data);

	//更新个人伤害排名数据
	void updatePlayerRankData(const Json::Value& data, eRankType type);

	//更新我的伤害排名数据
	void updateMyRankData(const Json::Value& data, eRankType type);

	//更新boss信息
	void updateBossInfo(const Json::Value& data);

	//奖励相关
	//解析boss宝箱奖励表
	void parseBossRewardData(int boxId, vector<sRewardItemData>& vecReward);

	//检测指定宝箱奖励ID是否属于不可领取状态
	bool isRewardCanNotGetState(int boxId);

	//定时器
	virtual void timeOut();

private:

	static GroupBossModel* ms_pInstance;

	//活动是否开启
	bool m_bIsActOpen;

	//boss是否可攻击
	bool m_bIsCanAtk;

	//是否要更新冷却时间
	bool m_bIsNeedUpdateCoolTime;

	//活动剩余时间
	int m_iRestTime;

	//鼓舞次数
	int m_iInspireTimes;

	//剩余挑战次数
	int m_iRestAtkTimes;

	//我的总累计伤害, 用来计算伤害奖励
	time_t m_iMyTotalDam;	

	//当前展示的boss宝箱对应的bossId
	int m_iRewardBossId;

	//伤害加成
	float m_fDamAdded;

	//boss相关数据
	sGroupBossInfo m_sBossInfo;

	//主界面用到排行
	vector<sGroupDamRank> m_vMainGroupRank;
	vector<sPlayerDamRank> m_vMainPlayerRank;
	
	//帮派排名数据
	vector<sGroupDamRank> m_vGroupDamRank;

	//个人全服排名数据
	vector<sPlayerDamRank> m_vPlayerDamRank;

	//个人帮派内部排名
	vector<sPlayerDamRank> m_vMemberDamRank;

	//我的帮派排名数据
	sGroupDamRank m_sMyGroupDamRank;

	//我的个人全服排名数据
	sPlayerDamRank m_sMyDamRank;

	//我的帮派内部排名数据
	sPlayerDamRank m_sMyInGroupDamRank;

	//boss宝箱奖励数据
	vector<sBoxReward> m_vBoxReward;

	//伤害宝箱奖励
	vector<sDamReward> m_vDamReward;

	//兑换
	vector<sExchangeJn> m_vExchangeJn;

	//鼓舞金币消耗
	vector<int> m_vInspireCost;
};

#endif