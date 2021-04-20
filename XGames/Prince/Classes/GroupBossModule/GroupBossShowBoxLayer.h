/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LimitShowBoxLayer
 *  Description:	
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef LimitShowBoxLayer_h__
#define LimitShowBoxLayer_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"

USING_NS_CC;

using namespace cocos2d::ui;	
using namespace std;


struct BOSS_SHOW_ONEITEM
{
	Layout	*Panel_reward;
};


struct BOSS_SHOW_UI_ROOT
{
	//根结点
	cocos2d::ui::Layout* uiRoot;

	std::vector<BOSS_SHOW_ONEITEM> _allItem;

};

class GroupBossShowBoxLayer : public ModalLayer 
{
public:
	GroupBossShowBoxLayer();
	~GroupBossShowBoxLayer();
	static GroupBossShowBoxLayer* create(int boxId);
public:
	bool init(int boxId);

	virtual void onEnter();

	virtual void onExit();
private:
	//初始化各种UI
	void  initUI();
private:
	BOSS_SHOW_UI_ROOT	m_ui;

	int _boxId;
};

#endif //GroupBossShowBoxLayer__h__