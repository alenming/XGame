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

struct SHOWBOX_ONEITEM
{
	Layout	*Panel_reward;
	Text	*Label_itemnumber;

};


struct SHOW_BOX_UI_ROOT
{
	//根结点
	cocos2d::ui::Layout* uiRoot;

	std::vector<SHOWBOX_ONEITEM> _allItem;

	Text	*Label_number;//积分
};

class LimitShowBoxLayer : public ModalLayer 
{
public:
	LimitShowBoxLayer();
	~LimitShowBoxLayer();
	static LimitShowBoxLayer* create(int boxId);
public:
	bool init(int boxId);

	virtual void onEnter();

	virtual void onExit();
private:
	//初始化各种UI
	void  initUI();
private:
	SHOW_BOX_UI_ROOT	m_ui;

	int _boxId;
};

#endif //LimitShowBoxLayer_h__
