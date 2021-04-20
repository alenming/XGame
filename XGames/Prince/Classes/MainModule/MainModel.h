#ifndef MainModel_h__
#define MainModel_h__

#include "Utils/Observer.h"
#include "Communication/Json/json.h"
#include "Utils/LocalTimer.h"

//系统表体力精力所在ID
#define POWER_RES_TIME_SYSTEM_ID		10077
#define ENERGY_RES_TIME_SYSTEM_ID		10078

struct CHABGE_PRIZE_ITEM //奖励节点
{
	int id;
	int amount;
};
const int CHABGE_GOLD_ITEM_ID = 11001; //金币
const int CHABGE_COIN_ITEM_ID = 11002; //银币
const int CHABGE_PVP_ITEM_ID = 11003; //竞技场积分
const int CHABGE_SHILIAN_ITEM_ID = 11004; //试炼积分
const int CHABGE_GROUP_ITEM_ID = 11005; //帮派积分
const int CHABGE_HUNTIAN_ITEM_ID = 11006; //浑天丹
const int CHABGE_JIUZHUAN_ITEM_ID = 11007; //九转丹
const int CHABGE_VIPEXP_ITEM_ID = 11008; //VIP经验
const int CHABGE_CUILIAN_ITEM_ID = 11009; //淬炼碎片
const int CHABGE_EXP_ITEM_ID = 11010; //主角经验

class MainParam
{

public:

	MainParam()
		: mRoleId(0)
		, mGold(0)
		, mMaxVipExp(0)
		, mMaxEnergy(0)
		, mMaxPower(0)
		, mPower(0)
		, mEnergy(0)
		, mVipExp(0)
		, mVipLevel(0)
		, mCoin(0)
		, mFinishedExpId(0)
		, mStarNum(0)
		, mMazeId(0)
		, mMazePower(0)
		, mHunTiandan(0)
		, mJiuZhuandan(0)
		, mPowerRestoreTime(0)
		, mEnergyRestoreTime(0)
		, mTotalPay(0)
		, mGroupId(0)
		, mGroupMoney(0)
		, mCuiLianMoney(0)
		, mPvpScore(0)
		, mTrialScore(0)
	{}
	~MainParam(){}

public:

	//主角名称
	string mName;

	//角色id
	int mRoleId;

	//当前元宝数量
	int mGold;

	//Vip当前等级最大经验
	int mMaxVipExp;

	//最大精力上限
	int mMaxEnergy;

	//最大体力上限
	int mMaxPower;

	//当前体力值
	int mPower;

	//当前精力值
	int mEnergy;

	//当前vip经验
	int mVipExp;

	//当前vip等级
	int mVipLevel;

	//当前游戏币数量
	int mCoin;

	//当前完成的阅历Id
	int mFinishedExpId;

	//关卡星级总数量
	int mStarNum;

	//迷宫行动力
	int mMazeId;
	int mMazePower;

	//浑天丹
	int mHunTiandan;

	//九转丹
	int mJiuZhuandan;

	//体力恢复时间
	int mPowerRestoreTime;

	//精力恢复时间
	int mEnergyRestoreTime;

	//角色性别, 1为男性, 0为女性
	int mSex;

	//玩家的支付总额，单位RMB
	int mTotalPay;

	//帮派id
	int mGroupId;

	//帮派名称
	string mGroupName; 

	//帮派货币
	int mGroupMoney;
	
	//淬练碎片
	int mCuiLianMoney;

	// 竞技场积分;
	int mPvpScore;

	// 试炼积分;
	int mTrialScore;
};

// 游戏交易状态;
class TradingStatus
{
public:
	TradingStatus()
		: mPowerNum(0)
		, mEnergyNum(0)
		, mActionNum(0)
		, mCoinNum(0)
		, mActivityNum(0)
		, mTowerResetNum(0)
		, mArenaShopRefreshNum(0)
		, mTrailShopRefreshNum(0)
		, mFactionShopRefreshNum(0)
		, mRefineShop8RefreshNum(0)
		, mRefineShop9RefreshNum(0)
		, bEnableSweep(false)
		, mZahuoRefrashNum(0)
		, mMysteryRefrashNum(0)
	{};
	~TradingStatus() {};

public:

	// 体力已购买次数;
	int  mPowerNum;

	// 精力已购买次数;
	int  mEnergyNum;

	// 迷宫行动力已购买次数;
	int  mActionNum;

	// 银币已购买次数;
	int  mCoinNum;

	// 活动挑战已购买次数;
	int  mActivityNum;

	// 爬塔重置已购买次数;
	int  mTowerResetNum;

	// 竞技场商店已刷新次数;
	int  mArenaShopRefreshNum;

	// 试炼商店已刷新次数;
	int  mTrailShopRefreshNum;

	// 帮派商店已刷新次数;
	int  mFactionShopRefreshNum;

	// 炼化炉 - 八卦商店已刷新次数;
	int  mRefineShop8RefreshNum;

	// 炼化炉 - 九宫商店已刷新次数;
	int  mRefineShop9RefreshNum;

	//杂货商店刷新次新
	int mZahuoRefrashNum;

	//神秘商店刷新次数
	int mMysteryRefrashNum;

	// 是否开启扫荡;
	bool  bEnableSweep;
};

class MainModel : public Observable, public TimeLocalListener
{

public:

	MainModel();
	virtual ~MainModel();

	static MainModel* getInstance();
	static void destroyInstance();

public:

	//获取角色数据
	MainParam* getMainParam();

	// 获取交易状态;
	TradingStatus* getTradingStatus();

	//初始化数据
	void initData(const Json::Value& data);
	//更新数据(需要显示货币奖励)
	void updateData(const Json::Value& data,vector<CHABGE_PRIZE_ITEM>* changPrizeItem = nullptr); 

	//打开体力精力计时器
	void openTimer();

	//设置金币数量，isBuy标明来源是否是购买获取
	void setGold(int num, bool isBuy = false);

	//设置银币数量
	void setSilver(int num);

	//设置体力
	void setPower(int num);

	//设置精力
	void setEnergy(int num);

	//设置帮派信息
	void setGroupInfo(int groupId, const string groupName);

	//设置帮贡货币
	void setGroupMoney(int num);

	//设置浑天丹
	void setHunTiandan(int num);

	//设置九转丹
	void setJiuZhuandan(int num);

	//设置淬炼碎片
	void setCuiLianMoney(int num);

	//vip经验
	void setVipExp(int num);

	//vip等级
	void setVipLv(int num);

	// 更新竞技场积分;
	void setPvpScore(int num) { mMainParam.mPvpScore = num; };

	// 更新试炼积分;
	void setTrialScore(int num) { mMainParam.mTrialScore = num; };

	//setPower/setEnergy之后重新计算监听器状态
	void setTimerState();

	//获取月卡开关状态, 返回true为开启
	bool getMonthCardIsOpen();

	//获取激活码开关状态, 返回true为开启
	bool getActCodeIsOpen();

	//处理体力计时
	void doPowerTimerEvent(int diffTime);

	//处理精力计时
	void doEnergyTimerEvent(int diffTime);

	void timeTick(int dt);

private:

	static MainModel* mInstance;

	//主角信息
	MainParam mMainParam;
	// 交易状态;
	TradingStatus mTradingStatus;

//体力、精力恢复相关

public:

	//体力、精力恢复满需要的总时间
	int powerTime;
	int energyTime;

	//boss开启状态
	int mBossActOpenState;

private:

	
	int mPowerDvalue;				//当前点的体力恢复需要的时间
	int mEnergyDvalue;				//当前点的精力恢复需要的时间

	//记录登录时的服务器时间戳
	time_t mtLoginServerTime;

	int mPowerResByTime;			//每xxx秒恢复一点体力
	int mEnergyResByTime;			//每xxx秒恢复一点精力

	//登录时记录的本地时间
	time_t mLoginLocalTime;

	//月卡开关
	bool mIsMonthCardOpen;

	//激活码开关
	bool mIsActCodeOpen;
};





#endif //MainModel_h__


