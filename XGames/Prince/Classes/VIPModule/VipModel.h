#ifndef VipModel_h__
#define VipModel_h__

#include "cocos2d.h"
#include "Communication/Json/json.h"
#include "Utils/Observer.h"
#include "VipDataDef.h"
#include "Utils/LocalTimer.h"


class VipModel : public Observable, public TimeLocalListener
{
public:
	static VipModel* getInstance();
	
	static void destroyInstance();

	//本地数据初始化
	void initFromLocal();

	//服务端数据初始化
	void initFromSvr(const Json::Value& vipData);

	//获取当前vip等级
	int getCurVipLv();

	//获取当前vip进度
	int getCurVipExp();

	//获得当前vip等级最大经验值
	int getCurVipMaxExp();

	//根据vip等级获取对应vip信息
	sVipData getVipInfo(int vipId);

	//获取各等级vip信息向量
	vector<sVipData>& getVecVipInfo();

	//获取各充值金币按钮上的信息
	vector<sRechargeData>& getVecRechargeInfo();

	//是否开启了充值功能
	bool getIsOpenPay();

	//记录本次购买的vip礼包id
	void setCurBuyVipBoxId(int vipId);
	//获取本次购买的vip礼包id
	int getCurBuyVipBoxId();

	//充值结果推送
	void payResultPush(const Json::Value& payResultData);

	//月卡剩余天数
	int& getMonthCardResDays();
	//设置月卡剩余天数
	void setMonthCardResDays(int days);

	time_t& getMonthCardOutTime();
	void	setMonthCardOutTime(time_t outTime);

	//充值成功更新到本地充值总额
	void onPaySuc(std::string orderId);
    
    // AppStore支付成功通知;
    void onAppStorePaySuc(std::string orderId);

	//是否拉取过数据
	bool& getIsInitted();

	void setCurPayRmb(int rmb);

	//获取是否有vip礼包可以领取
	bool isCanGetVipBox();

	void timeTick(int dt);
    
    // 记录订单号;
    void setOrderId(std::string orderId);

private:
	VipModel();
	virtual ~VipModel();

private:

	static VipModel* ms_pInstance;

	//各等级vip信息
	vector<sVipData> m_vecVipInfo;

	//充值金币按钮选项上的信息
	vector<sRechargeData> m_vecRechargeBtnInfo;
	
	//当前vip等级
	int m_curVipLv;

	//当前vip进度
	int m_curVipExp;

	//当前vip等级最大进度值
	int m_curVipMaxExp;

	//是否开启了充值功能
	bool m_bIsOpenPay;

	//记录本次购买的vip礼包id
	int m_curBuyVipBoxId;

	//剩余的月卡天数
	int mMonthCardResDays;	
	//服务器月卡截止时间戳
	time_t mMonthCardOutTime;

	//是否拉取过数据
	bool m_bIsInitted;

	int m_curPayRmb;
    
    vector<string>  m_vcOrderUnPurchased;
};

#endif