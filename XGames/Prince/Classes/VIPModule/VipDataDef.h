#ifndef VipDataDef_h__
#define VipDataDef_h__

#define TOTAL_VIP_LV				15
#define VIP_LV_BTN0_TAG				176
#define VIP_BUY_GOLD_BTN1_TAG		197

#define MONTH_CARD_RES_DAYS			555555

//VIP特权可购买的礼包
typedef struct VipTeQuanBox {
	int mboxId;
	int mCount;
}sVipSpecBox;

struct OrderInfo
{
	string orderId;
	string extension;
};

//VIP等级对应数据，针对充值界面需要，暂时列这么多
typedef struct VipData {
	
	int mVipId;

	//VIP各等级需要的充值金币数
	int mVipMaxExp;

	//Vip礼包原价
	int mVipBoxOldPrice;

	//Vip礼包特价
	int mVipBoxPrice;

	//Vip 等级
	string mVipLv;

	//Vip特权描述
	string mVipSpecDetails;

	//该等级Vip特权礼包是否已经领取过
	bool mIsTaked;

	//Vip 特权礼包
	vector<sVipSpecBox> mVecVipSpecBox;
}sVipData;

//充值数据
typedef struct rechargeData
{
	int id;					//id
	int channel;			//渠道号
	int money;				//rmb价格
	int payGold;			//购买的金币数量
	int resId;				//图标资源
	int mark;				//剩余x天，永久，双倍标签标识
	string markDes;			//立返描述
	string des;				//福利描述
	bool rechargedFlag;		//根据这个显示标签 false, 标签显示， true标签不显示(已买过)

	rechargeData()
		:id(-1)
		,channel(-1)
		,money(0)
		,payGold(0)
		,resId(-1)
		,mark(0)
		,markDes("")
		,des("")
		,rechargedFlag(false) {}
}sRechargeData;

enum Panel_Type
{
	VIP_VIEW,		//vip面板
	PAY_VIEW		//充值面板
};

#endif