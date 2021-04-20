/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerGoldActivity
 *  Description:	天降金幣模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			05/21/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef  LayerGoldActivity_h__
#define  LayerGoldActivity_h__

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
#include "GoldActivityModel.h"
#include "GoldActivityController.h"
#include "GoldActivityDef.h"
#include "Widget/ActionCreator.h"
#include "Temp/CustomTips.h"
#include "GoldActivityModel.h"
#include "Common/Common.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"
#include "Widget/RollNum.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class GoldActivityLayer :public Layer,public Observer
{
public:
	~GoldActivityLayer(void);
	CREATE_FUNC(GoldActivityLayer);

	//数据更新
	virtual void updateSelf(void* data);
	//初始化
	virtual bool  init();

	virtual void onEnter();
	virtual void onExit(); 
	void updateNextFram(float ft);

	//按钮回调
	void  onBtnClick( Ref* sender, Widget::TouchEventType type );

	void showDownTime();
	//活动时间格式化：天、时、分、秒
	void formatDetailsTime(int secs, GoldActivityFormatTime& time);

	void setPanelInfo();//更新界面
	void setSpendGoldInfo();

	void setGetGold();//搖到錢
	void setOtherGetGold();//別人也搖到錢

	void getMoney(Ref* sender);
	//show奖励
	void showPopReward();
private:
	GoldActivityLayer(void);

	//初始化各种UI
	void  initUI();
private:
	Ui_GoldActivity m_ui; //总UI数据

	RollNumGroup *m_rollNum;
};

#endif
