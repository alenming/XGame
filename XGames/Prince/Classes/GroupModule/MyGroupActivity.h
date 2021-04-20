/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupLayer
 *  Description:	帮派成员操作的Layer实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef MyGroupActivity_h__
#define MyGroupActivity_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;

enum ACTIVITY_CALLBACK_TYPE
{
	ACT_CB_RULE,
	ACT_CB_HISTORY,
	ACT_CB_BEGIN

};

struct UI_GroupActivity
{
	//根结点
	cocos2d::ui::Layout* uiRoot;

	//按钮
	cocos2d::ui::Button* uiBtnRule;
	cocos2d::ui::Button* uiBtnHistory;
	cocos2d::ui::Button* uiBtnBegin;
	cocos2d::ui::Button* uiBtnRefresh;
	//可获得物品展示
	std::vector<Layout*> uiLayoutshowItem;
	
	cocos2d::ui::Layout* uiLayoutshowHero;

	std::vector<Layout*> uiLayoutNineItem;

	cocos2d::ui::Text*   uiTextFindCount;

	Text* Lab_Go;
	ImageView* Img_Free;
	ImageView* Img_Gold;
	Text* Lab_Gold;

	Text* Lab_Over;
};

class MyGroupActivity : public ModalLayer,public Observer
{
public:
	MyGroupActivity();
	~MyGroupActivity();
	static Layer* create();
	virtual void updateSelf(void* data);
public:
	bool init();

	void update(float dt);

	virtual void onEnter();

	virtual void onExit();
private:
	//初始化各种UI
	void  initUI();

	void  onBtnClickRefresh( Ref* reff, Widget::TouchEventType type);

	void  onBtnClickByListId( Ref* reff, Widget::TouchEventType type, int listId);

	void  onBtnClick( Ref* reff, Widget::TouchEventType type, ACTIVITY_CALLBACK_TYPE cbType);


	void updateNextFram(float ft);

	//初始化公共展示UI
	void updatePublicUI();

	//初始化九个格子UI
	void updateNineItemUI();

	//根据状态初始化那些UI
	void updateOtherUI();

	//播放点击开始寻龙后翻转动画
	void playStartAnimation();

	//播放点击之后物品物品谈出消失动画
	void playItemOutAnimation();

	//9个格子进场动画
	void playNineItemInAnimation();

	//英雄骨骼动画
	void showHeroAni();

	void showThreeItem();

	void updateOneItemAction();

	void showRewardGetted();
private:
	UI_GroupActivity m_ui;

	int weizhi;

	bool isRun;

	bool firstXunlong;

	SpineAnimation* pHero;
};

#endif //MyGroupActivity_h__
