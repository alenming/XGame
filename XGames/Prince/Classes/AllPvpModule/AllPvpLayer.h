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
#include "AllPvpDef.h"
#include "Widget/ActionCreator.h"
#include "Temp/CustomTips.h"
#include "Common/Common.h"
#include "Temp/CustomGoldTips.h"
#include "SDK_TalkingData/TalkingData.h"
#include "Widget/RollNum.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class AllPvpLayer :public Layer
{
public:
	~AllPvpLayer(void);
	CREATE_FUNC(AllPvpLayer);
	//初始化
	virtual bool  init();
	void  setListView();
	bool openLv(int type,int& lv);

	virtual void onEnter();
	virtual void onExit(); 

private:
	AllPvpLayer(void);

	//初始化各种UI
	void  initUI();
private:
	tagUI_AllPvp m_ui; //总UI数据

};

#endif
