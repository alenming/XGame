/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSign
 *  Description:	每日签到模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			06/10/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef  LayerSign_h__
#define  LayerSign_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "ResourceManager/ResourceLoader.h"
#include "ui/UIDef.h"
#include "ui/ModalLayer.h"
#include "Utils/Observer.h"
#include "FormationModule/CustomScrollBar.h"
#include "ToolModule/UIToolHeader.h"
#include "ToolModule/ToolModel.h"
#include "Utils/StringFormat.h"
#include "LayerSignModel.h"
#include "LayerSignController.h"
#include "Widget/ActionCreator.h"
#include "Widget/SpineAnimation.h"
#include "HeroModule/SceneAllHero.h"
#include "Temp/CustomTips.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

#define SEVEN_DAY 7
#define DELAY_TIME 3.0f


//面板控件集合
typedef struct tagUI_Sign
{
	Layout* pRoot;
	Button* btnClose; //关闭按钮
	Button* btnHelp;  //说明按钮
	ImageView*  imgMonth; //月份图片
	TextAtlas*  signCount;//签到次数
	cocos2d::ui::ScrollView* scrollView;//奖励物品面板

	Layout* hero;//英雄
	ImageView *canotGet; //不能招募
	ImageView *canGet; //能招募
	ImageView *gettEd; //已能招募
	Text*  signHero; //本月签到英雄
	Text* tips; //提示
	Text* tipsCount; //提示个数
	Text* curCount; //当前个数


	Button* btnGet; //领取按钮

	tagUI_Sign()
	{
		memset(this, 0, sizeof(tagUI_Sign));
	}
}Ui_Sign;

struct UIItemParam
{
	UIToolHeader* icon;
	LayerSignParam* parm;
};

const int SIGN_GOLD_ITEM_ID = 11001;
const int SIGN_COIN_ITEM_ID = 11002;

class LayerSign : public ModalLayer,public Observer
{
public:
	~LayerSign(void);
	CREATE_FUNC(LayerSign);

	//初始化
	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 

	//按钮回调
	void  onBtnClick( Ref* ref, Widget::TouchEventType type, int nWidgetName );

	//数据更新
	virtual void updateSelf(void* data);

	//物品栏消息相应
	void onScrollViewEvent(Ref* pSender, cocos2d::ui::ScrollView::EventType type);

	//下拉物品
	void initScrollView();
	void setGetHeroInfo(); //设置招募英雄信息
	//添加物品到签到面板
	void setSignPanelList();
	void changSignPanelListItem(int idx);

	//show奖励
	void showPopReward();
private:
	LayerSign(void);

	//初始化各种UI
	void  initUI();
private:
	Ui_Sign m_ui; //总UI数据
	int m_signCount; //目前签到天数
	int m_signMonth;//目前签到月份
	int m_canSign; //今天是否可以签到

	CustomScrollBar* m_customScrollBar1; //滚动条
	CustomScrollBar* m_customScrollBar2; //滚动条
	CustomScrollBar* m_customScrollBar3; //滚动条

	//按钮回调相应事件 
	enum Ui_Sign_Widget
	{
		Btn_Close,
		Btn_Help,
	};

	//按钮回调相应事件 
	enum DELAY_CALL
	{
		RI_CALL,
		YUE_CALL,
	};
};

#endif
