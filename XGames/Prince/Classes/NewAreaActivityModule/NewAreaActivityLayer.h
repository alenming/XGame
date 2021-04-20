/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		NewAreaActivityLayer
 *  Description:	新区活动UI;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			26/10/2015
 *  Others: 
 *
 ******************************************************************/

#ifndef NewAreaActivityLayer_h__
#define NewAreaActivityLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "NewAreaActivityController.h"
#include "FormationModule/CustomScrollBar.h"
#include "Utils/LocalTimer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

typedef struct actItem
{
	Layout* pnlItem;
	Text*	taskContent;
	Text*	taskGoal;
	vector<Layout*> vecPnlProp;
	Button* btnGet;
	Button* btnGo;
	ImageView* imgGettedFlag;
	bool isGetted;			//是否已领取，领取过的按照id排列放列表后面
	bool isCanGet;			//是否可领取，可领取的放在前面
	int rewardId;
}sActItem;

typedef struct tagUI_NewAct
{
	//根节点
	Layout*		pRoot;

	//活动倒计时
	Text*	txtActTime;

	//滚动面板
	ui::ScrollView* pnlScrollView;
	//滚动列表子项
	Layout* pnlItem;

	//特价抢购面板
	ImageView* imgBuyProp;
	Layout* pnlProp;			//特价道具容器
	Text*	txtPropName;
	Text*	txtOldPrice;
	Text*	txtNewPrice;
	Button*	btnBuy;
	ImageView* imgBought;
	Text*   txtLimitAmount;		//数量限制提示
	Text*	txtLimitSurplus;	//剩余个数

	tagUI_NewAct()
	{
		memset(this, 0, sizeof(tagUI_NewAct));
	}

}sTagUI_NewAct;

enum UI_BTN_FALG
{
	Btn_Buy,
	Btn_pnl1,
	Btn_pnl2,
	Btn_pnl3,
	Btn_pnl4,
	Btn_Day1,
	Btn_Day2,
	Btn_Day3,
	Btn_Day4,
	Btn_Day5,
	Btn_Day6,
	Btn_Day7,
	Btn_Get,
	Btn_Go
};

class NewAreaActivityLayer : public Layer, public Observer, public TimeLocalListener
{
public:
	NewAreaActivityLayer();
	~NewAreaActivityLayer(void);

	CREATE_FUNC(NewAreaActivityLayer);

public:
	virtual bool  init();
	virtual void onEnter();
	virtual void onExit(); 
	void update(float dt);

private:
	void initUI();

	//初始化特价抢购面板
	void initBuyPropView();

	//刷新活动面板
	void updatePnlAct(int nWidgetName);

	//刷新滚动面板
	void updateScrollView(int nWidgetName);

	//刷新特价抢购面板
	void updateBuyView();

	//刷新标签按钮文字
	void updateBtnLabel();
	//刷新标签高亮
	void updateBtnLabelHighLight(Button* btn);

	//拉取数据主界面后处理当天按钮状态
	void updateCurDayState();

	//倒计时
	void updateResTime(bool isActEnd);

	//格式化时间  xx天xx小时xx分xx秒
	string formatTime(int sec);

	//小红点显示
	void showRedFlag();

	//领取奖励后提示
	void showGetReward();

	//页面跳转
	void goToOtherScene(int sceneId);

	void onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);

	//开启定时器
	void openUITimer();
	virtual void timeTick(int dt);

	virtual void updateSelf(void* data);

private:
	sTagUI_NewAct m_ui;

	vector<Button*> m_vBtnLabel;
	vector<Button*> m_vBtnDays;
	vector<sActItem> m_vItemList;

	//活动列表滚动条
	CustomScrollBar* m_BarActItemList;

	//当前选中第几天的活动(0~6)
	int m_iCurSelectedDay;
	//当前选中的标签
	int m_iCurSelectedLabel;

	Layout* m_pnlGettedRewardItem;

	//活动剩余时间(s)
	int m_iResTime;

	//标记领取的ID
	int m_iSelectedId;

	//标记离开场景时的按钮
	int m_iLeaveBtnId;
	//标记离开场景时的标签
	int m_iLeaveLabId;
};


#endif