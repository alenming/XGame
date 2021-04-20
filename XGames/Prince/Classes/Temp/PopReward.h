/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		PopReward
 *  Description:	获得奖励的公共弹窗;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			06/03/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef PopReward_h__
#define PopReward_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "cocostudio.h"
#include "UI/UIDef.h"
#include "Common/Common.h"
#include "ToolModule/UIToolHeader.h"
#include "Utils/ResourceUtils.h"
#include "DataManager/DataManager.h"
#include "DataManager/DataTable.h"
#include "Utils/SoundUtils.h"
#include <algorithm>

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

enum Pop_Reward_Type
{
	PRT_COIN,		// 银币;
	PRT_GOLD,		// 金币;
	PRT_EXP,		// 经验;
	PRT_TOOL,		// 道具;
	PRT_POWER,		// 体力;
	PRT_BANGONG,    // 帮贡
	PRT_PVPSCORE,   // 竞技场积分
	PRT_TRIALSCORE, //试炼积分
	PRT_VIPEXP,     //vip经验
	PRT_HUNTIAN,    //浑天丹
	PRT_JIUZHUAN,   //九转丹
	PRT_CUILIAN,    //淬炼丹
	PRT_TREASURE,	// 藏宝图;
	PRT_FRIEND_SHIP, //友情点
	PRT_INVALID		// 不可用;
};

enum PopRewardEventType
{
	EVENT_TYPE_SHOW,	// 展示(渐入动画完成);
	EVENT_TYPE_END		// 结束(渐出动画完成);
};

// 回调函数类型;
typedef std::function<void(Ref*, PopRewardEventType)> PopRewardCallback;

// 奖励物品基类;
class PopRewardItem
{
public:
	PopRewardItem(const int count = 0)
		: nCount(count)
	{};
	virtual ~PopRewardItem() {};

	virtual Pop_Reward_Type getType() = 0;
	virtual string getResId() = 0;
	virtual string getName() = 0;
	virtual int  getCount() { return nCount; };

	virtual int  getId() { return -1; };

protected:
	int nCount;
};

//友情点
class PopRewardItem_FriendShip : public PopRewardItem
{
public:
	PopRewardItem_FriendShip(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_FriendShip() {};

	virtual Pop_Reward_Type getType() { return PRT_FRIEND_SHIP; };
	virtual string getResId() { return "10000"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20108); };
};


//体力
class PopRewardItem_Power : public PopRewardItem
{
public:
	PopRewardItem_Power(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_Power() {};

	virtual Pop_Reward_Type getType() { return PRT_POWER; };
	virtual string getResId() { return "power"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20109); };
};

//帮贡
class PopRewardItem_BanGong : public PopRewardItem
{
public:
	PopRewardItem_BanGong(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_BanGong() {};

	virtual Pop_Reward_Type getType() { return PRT_BANGONG; };
	virtual string getResId() { return "11005"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20110); };
};

//竞技场积分
class PopRewardItem_PvpScore : public PopRewardItem
{
public:
	PopRewardItem_PvpScore(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_PvpScore() {};

	virtual Pop_Reward_Type getType() { return PRT_PVPSCORE; };
	virtual string getResId() { return "11003"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20111); };
};

//试炼积分
class PopRewardItem_TrialScore : public PopRewardItem
{
public:
	PopRewardItem_TrialScore(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_TrialScore() {};

	virtual Pop_Reward_Type getType() { return PRT_TRIALSCORE; };
	virtual string getResId() { return "11004"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20112); };
};

//Vip经验
class PopRewardItem_VipExp : public PopRewardItem
{
public:
	PopRewardItem_VipExp(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_VipExp() {};

	virtual Pop_Reward_Type getType() { return PRT_VIPEXP; };
	virtual string getResId() { return "11008"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20113); };
};

//浑天丹
class PopRewardItem_HunTian : public PopRewardItem
{
public:
	PopRewardItem_HunTian(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_HunTian() {};

	virtual Pop_Reward_Type getType() { return PRT_HUNTIAN; };
	virtual string getResId() { return "11006"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20114); };
};

//九转丹
class PopRewardItem_JiuZhuan : public PopRewardItem
{
public:
	PopRewardItem_JiuZhuan(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_JiuZhuan() {};

	virtual Pop_Reward_Type getType() { return PRT_JIUZHUAN; };
	virtual string getResId() { return "11007"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20115);};
};

// 淬炼;
class PopRewardItem_CuiLian : public PopRewardItem
{
public:
	PopRewardItem_CuiLian(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_CuiLian() {};

	virtual Pop_Reward_Type getType() { return PRT_CUILIAN; };
	virtual string getResId() { return "11009"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20116); };
};

// 金币;
class PopRewardItem_Gold : public PopRewardItem
{
public:
	PopRewardItem_Gold(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_Gold() {};

	virtual Pop_Reward_Type getType() { return PRT_GOLD; };
	virtual string getResId() { return "gold"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20117); };
};

// 银币;
class PopRewardItem_Coin : public PopRewardItem
{
public:
	PopRewardItem_Coin(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_Coin() {};

	virtual Pop_Reward_Type getType() { return PRT_COIN; };
	virtual string getResId() { return "coin"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20118); };
};

// 经验;
class PopRewardItem_Exp : public PopRewardItem
{
public:
	PopRewardItem_Exp(const int count = 0)
		: PopRewardItem(count)
	{};
	~PopRewardItem_Exp() {};

	virtual Pop_Reward_Type getType() { return PRT_EXP; };
	virtual string getResId() { return "exp"; };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20119); };
};

// 藏宝图;
class PopRewardItem_CangBao : public PopRewardItem
{
public:
	PopRewardItem_CangBao(const int id,const int count = 0)
		: PopRewardItem(count),
		nId(id)
	{};
	~PopRewardItem_CangBao() {};

	virtual Pop_Reward_Type getType() { return PRT_TREASURE; };
	virtual string getResId() { return "cangbao"+TO_STR(nId); };
	virtual string getName() { return DataManager::getInstance()->searchCommonTexdtById(20120); };

	virtual int getId() { return nId; };

public:
	int  nId;
};

// 道具;
class PopRewardItem_Tool : public PopRewardItem
{
public:
	PopRewardItem_Tool(const int id = -1, const int count = 0)
		: PopRewardItem(count)
		, nId(id)
	{};
	~PopRewardItem_Tool() {};

	virtual Pop_Reward_Type getType() { return PRT_TOOL; };

	virtual string getResId()
	{
		string resId("-1");
		RowData* _row = DataManager::getInstance()->searchToolById(nId);
		if (nullptr != _row)
		{
			int nResId = _row->getIntData("resId");
			resId.clear();
			resId.assign(StringFormat::intToStr(nResId));
		}
		return resId;
	};

	virtual string getName()
	{
		string name("");
		RowData* _row = DataManager::getInstance()->searchToolById(nId);
		if (nullptr != _row)
		{
			name = _row->getStringData("itemName");
		}
		return name;
	};

	virtual int getId() { return nId; };

public:
	int  nId;
};

class PopReward : public LayerColor
{
public:
	~PopReward(void);

	static void  show(const vector<PopRewardItem*>& vcReward, PopRewardCallback callBackFunc = nullptr);

	static PopReward*  create(const vector<PopRewardItem*>& vcReward, PopRewardCallback callBackFunc = nullptr);

	virtual bool  init(const vector<PopRewardItem*>& vcReward, PopRewardCallback callBackFunc = nullptr);

	virtual void  update(float delta);

	virtual void  onEnter();

private:
	PopReward(void);

	void  initUI();

	// 获得物品动画;
	void  showItemAnimation(Node* pSender, int nIndex, bool bAuto = true);

	// 点击继续;
	void  showTouchContinue();

private:
	Layout*		m_pRoot;
	//ImageView*	m_imgTitle;
	ImageView*  m_imgBg;
	Layout*		m_pnlContent;
	ImageView*	m_imgContinue;

	vector<PopRewardItem*>  m_vcReward;

	int  m_nCurIndex;
	int  m_nCurPartCount;

	bool m_bIsEnableContinue;

	PopRewardCallback _callBackFunc;
};

#endif // PopReward_h__
