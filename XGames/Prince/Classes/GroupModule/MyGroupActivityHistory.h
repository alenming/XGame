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
#ifndef MyGroupActivityHistory_h__
#define MyGroupActivityHistory_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "MyGroupDataDef.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;



struct UI_ActivityHistory
{
	//根结点
	cocos2d::ui::Layout* uiRoot;

	cocos2d::ui::ScrollView* ScrollView_1;

	cocos2d::ui::Button*	Btn_Close;

	cocos2d::ui::Layout* Pnl_List;


	
};

class MyGroupActivityHistory : public ModalLayer ,public Observer
{
public:
	MyGroupActivityHistory();
	~MyGroupActivityHistory();
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

	void updateUI();

	void  onBtnClick( Ref* reff, Widget::TouchEventType type);


	void updateNextFram(float ft);
	void onScrollViewEvent(Ref* pSender, ui::ScrollView::EventType type);
	
private:
	UI_ActivityHistory m_ui;

	//历史列表滚动条
	CustomScrollBar* m_HisttoryScrollBar;
};

#endif //MyGroupActivityHistory_h__
