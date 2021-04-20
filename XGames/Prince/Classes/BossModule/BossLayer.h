#ifndef BossLayer_h__
#define BossLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "BossController.h"
#include "Widget/SpineAnimation.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef struct fightResultRank
{
	Text*	txtPlayerName;
	Text*	txtPlayerDamage;
	int rank;
}sFightResultRank;

typedef struct bossInfo
{
	Layout*		pnlBoss;
	ImageView*	imgBossAttribute;			//Boss属性
	LoadingBar*	barHp;
	ImageView*	imgBossImage;
	ImageView*	imgKilledFlag;				//击杀标识
	Text*		txtKillerName;				//击杀者
	Text*		txtBossLevel;
	Text*		txtBossName;
	int			bossId;
	bool		isLive;						//boss是死是活
}sBossInfo;

typedef struct lastResultRankInfo
{
	Layout* pnlRankItem;
	ImageView* imgRankBg;
	Text*	txtRankTxt;
	Layout*	pnlPlayerHead;
	Text*	txtPlayerName;
	Text*	txtDamage;
}sLastResultRankInfo;

typedef struct rewardInfo
{
	Layout* pnlRewardItem;
	Text*	txtDamageGoalTips;
	Text*	txtDamageGoal;
	Text*	txtMyTotalDamage;
	Layout*	pnlProp1;
	Layout* pnlProp2;
	Layout* pnlProp3;
	Layout* pnlProp4;
	Button* btnGet;
	ImageView* imgGettedFlag;
	int rewardId;
}sRewardInfo;

typedef struct rewardTool
{
	int id;
	int count;
}sRewardTool;

typedef struct tagUI_Boss
{
	//根节点
	Layout*		pRoot;	

	//未开启层
	Layout*	layerNotOpen;					//活动未开启
	Text*	txtRankFrist;					//排名1
	Text*	txtRankSecond;					//排名2
	Text*	txtRankThird;					//排名3
	Text*	txtCoolDown;					//反击倒计时
	Button*	btnLastTimeResult;				//上轮战绩

	Layout* layerLastResult;				//上轮战绩列表
	Layout* pnlLastResultItem;
	Text*	txtMyRank;
	Text*	txtMyDamage;

	Button*	btnClose1;
	ui::ScrollView*	scrollRankList;

	//战斗层
	Layout*	layerActivity;					//活动层
	vector<sFightResultRank> vecResultText;
	vector<sBossInfo>	vecBossInfo;
	Text*	txtNotInRank;					//未上榜提示
	Text*	txtRestTime;					//反击剩余时间
	Text*	txtCoolTime;
	Text*	txtDamageAdded;					//伤害加深
	Text*	txtClearCoolCost;				//清除冷却花费
	Text*	txtInspireCost;					//鼓舞花费
	Layout*	pnlClearCool;					//清除冷却

	Layout* pnlBossKilledTips;
	Text*	txtBossKilledTips;
	Text*	txtBossKillerTips;

	//列表层
	Layout*	layerRewardListView;
	Layout* pnlRewardItem;
	ui::ScrollView*	scrollGetReward;
	Button* btnGetAllReward;				//一键领取

}sUI_Boss;

enum All_BtnFlag
{
	Btn_GetReward,
	Btn_Rules,	
	Btn_LastTimeResult,
	Btn_ClearCoolTime,
	Btn_Inspire,
	Btn_Close1,
	Btn_Close2,
	Btn_GetItemReward,
	Btn_GetAllReward,
	Btn_Boss1,
	Btn_Boss2,
	Btn_Boss3,
	Btn_Boss4
};

enum eTimeFormat
{
	eFormatHMS,			//00:00:00
	eFormatMS			//00:00
};

enum eBossAttribute
{
	eTypeFire = 1,
	eTypeWater,
	eTypeWood
};

class BossLayer : public Layer, public Observer
{
public:
	BossLayer();
	~BossLayer(void);

	CREATE_FUNC(BossLayer);

public:
	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 
	void update(float dt);

private:
	void initUI();

	//未激活
	void initUnactivatedUI();	

	//上轮战绩
	void initLastTimeResultUI();
	
	//激活状态
	void initActivityUI();

	//初始化四个boss骨骼动画
	void initBossSpine();
	
	//奖励列表
	void initRewardListUI();

	//刷新未开启界面
	void updateUnactivatedView();

	//刷新活动界面
	void updateActivityView();

	//卸载定时刷新界面定时器并且刷新界面
	void updateViewWithUnschedule();

	//刷新上轮战绩列表
	void updateLastRankListView();

	//刷新奖励列表
	void updateRewardListView();

	//boss被击杀
	void showBossKilledView();

	//领取奖励后提示
	void showGetReward();

	//秒数格式化成 00:00:00
	string formatTime(int sec, eTimeFormat format);

	//倒计时定时器
	void updateBySec(float dt);

	//定时刷新boss界面
	void updateActiveState(float dt);
	
	void onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	virtual void updateSelf(void* data);

private:
	sUI_Boss m_ui;

	//滚动条 1.伤害排行榜
	CustomScrollBar* m_RankScrollBar;

	//滚动条 2.奖励列表
	CustomScrollBar* m_RewardScrollBar;

	BossController m_bossController;

	int m_restTime;			//倒计时
	int m_coolTime;			//冷却倒计时

	//清除冷却消费
	int m_iClearCoolCost;
	vector<int> m_vecInspireCost;
	vector<sRewardInfo> m_vecRewardInfo;
	bool m_bIsGettedRewardOnce;		//是否一键领取
	Layout* m_pnlGettedRewardItem;
	bool m_bIsNeedConfirmInspire;
	//是否需要拉取前50名数据
	bool m_bIsNeedGetRank;

	//活动是否结束
	bool m_bIsActEnded;

	//四个boss骨骼动画
	vector<SpineAnimation*> m_vBoss;
	//是否初始化过boss骨骼动画
	bool m_bIsInittedSpine;
};


#endif