/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		FirstRecharge
 *  Description:	首充送礼模块;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			11/12/2015
 *  Others: 
 *
 ******************************************************************/


#ifndef FirstRecharge_h__
#define FirstRecharge_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "CocoStudio.h"
#include "UI/ModalLayer.h"
#include "Utils/Observer.h"
#include "MainModule/MainModel.h"
#include "FirstRechargeController.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

enum class FIRST_RECHARGE_BTN_TAG
{
	RECHARGE,
	GETREWARD,
	CLOSE
};

typedef struct tagUI_FirstRecharge 
{
	Layout*		pRoot;							//根节点

	Button*		rechargeBtn;
	Button*		getRewardBtn;
	Button*		closeBtn;

	tagUI_FirstRecharge()
	{
		memset(this, 0, sizeof(tagUI_FirstRecharge));
	}
}UI_FirstRecharge;

class FirstRechargeLayer : public ModalLayer, public Observer
{
public:
	FirstRechargeLayer();
	~FirstRechargeLayer();
	CREATE_FUNC(FirstRechargeLayer);

public:
	bool init();

	virtual void onEnter();
	virtual void onExit();

	//初始化UI
	void  initUI();

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	//按钮回调
	void  onBtnClick( Ref* ref, Widget::TouchEventType type, FIRST_RECHARGE_BTN_TAG nWidgetName );

	//展示奖励
	void showReward();

private:

	UI_FirstRecharge m_ui;

	FirstRechargeController mController;

	//奖励的英雄ID和碎片数量
	int m_iHeroTemplateId;
	int m_iChipCount;
};

#endif