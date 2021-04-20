#ifndef LimitTimeHeroLayer_h__
#define LimitTimeHeroLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "LimitTimeHeroController.h"
#include "LimitTimeHeroDataDef.h"
#include "LimitTimeHeroModel.h"
#include "LimitTimeHeroController.h"
#include "HeroModule/HeroModel.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

//限时神将UI结构
typedef struct tag_UILimitTimeHero
{
	Layout* pRoot;						//根节点

	//英雄展示节点
	ImageView*	imgHeroNick;
	Text*		txtHeroName;
	Text*		txtTenReward;
	Text*		txtOneBuy;
	Text*		txtTenBuy;
	//TextAtlas*	txtHeroTalent;
	ImageView*	imgHeroTalent;
	Layout*		pnlHeroAni;
	Button*		btnHelp;
	Button*		btnOneBuy;
	Button*		btnTenBuy;

	//奖励滚动展示节点
	ui::ScrollView*	scrollScoreRank;
	ui::ScrollView*	scrollRewardRank;
	Layout*		pnlScoreItemTemp;
	Layout*		pnlRewardItemTemp;

	//积分进度节点
	LoadingBar*	barScore;
	vector<Button*> vecScoreBox;

	//其它
	Text*		txtMyScore;
	Text*		txtMyRank;
	Text*		txtGold;
	Text*		txtResDays;		//活动剩余天数
	Text*		txtResTime;		//活动剩余时分秒
	Button*		btnClose;

}sTag_UILimitTimeHero;

//按钮标记
typedef enum UI_Btn
{
	BTN_HELP,
	BTN_BUYONE,
	BTN_BUYTEN,
	BTN_SCOREBOX,
	BTN_CLOSE

}eUI_Btn;

class LimitTimeHeroLayer : public Layer, public Observer, public TimeLocalListener
{
public:
	LimitTimeHeroLayer();
	virtual ~LimitTimeHeroLayer();

	CREATE_FUNC(LimitTimeHeroLayer);

public:
	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 
	void update(float dt);

private:
	void initUI();
	void updateUI();

	//倒计时
	void showDownTime();

	//英雄展示
	void showHeroAni();

	//排名奖励信息
	void upateRankRewardView();

	//更新积分排名和我的积分信息
	void updateScoreRankState();

	//刷新积分宝箱状态
	void updateRewardBoxState();

	void onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	//展示、领取积分宝箱
	void onBoxClicked(Button* btn); 

	//宝箱奖励领取后展示
	void showBoxRewardGetted();

	//抽卡、宝匣消息响应
	void onTakeCardBoxEvent();

	//添加抽道具层
	void addBuyLayerByType(eChouKaType type);

	//活动时间格式化：天、时、分、秒
	void formatDetailsTime(int secs, sFormatTime* strTime);

	//滚动层事件监听
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//宝箱可领取状态动画
	void showCanGetAni(Node* parentNode);
	//移除可领取状态动画
	void removeCanGetAni(Node* parentNode);

	//定时器.
	void openUITimer();
	void timeTick(int dt);

	virtual void updateSelf(void* data);

private:
	sTag_UILimitTimeHero m_ui;

	//积分排名滚动条
	CustomScrollBar* m_scoreBar;

	//奖励排名滚动条
	CustomScrollBar* m_rewardBar;

	//奖励英雄
	HeroParam* m_heroParam;

	LimitTimeHeroController m_Controller;

	//当前领取的宝箱ID
	int m_iBoxId;
};


#endif