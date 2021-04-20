/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsSuLayer
 *  Description:	星宿层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/10/2015
 *  Others:			
 *
 ******************************************************************/
#include "StarsSuHeader.h"
#include "StarsSuController.h"
#include "StarsSuModel.h"
#include "DataManager/DataTable.h"
#include "CCShake.h"

#ifndef StarsSuLayer_h_
#define  StarsSuLayer_h_

const int StarsSuCont = 7;
const float LoadTime = 0.5f;

class StarsSuLayer:public Layer,public Observer
{
public:
	~StarsSuLayer();

	static StarsSuLayer* create(bool pushScene);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);


	void upDateGetStarInfo(bool isInit = false,bool isShowAnimi = false); //更新拉取星宿信息界面

	void animationAstrology(); //占星完毕动画
	void animationBreakOut(); //突破完毕动画
	void animationChongpoOut(); //突破动画

	void animationChontOut(); //冲破动画


	void sendButtonMsg(Ref* sender,Widget::TouchEventType type,ASTROLOGY_TYPE_BUTTON kind);//占星 突破公用
	void lookStarInfo(Ref* sender,Widget::TouchEventType type,int idx);//查看每个星星对应增加属性


	void setAstrologyPalyerDesc();   //占星引起属性改变值
	void setBreakOutPalyerDesc(); //突破引起属性改变值

	void setAtrChangeAnimat(); //占星引起属性改变值弹出


	void setStarType(bool isInit); //星星展示
	void addAttributeItem(); //右边属性列表
	string getAttributeData(int id); //获得总体属性值

	void onScrollViewEvent(Ref* pSender, ScrollView::EventType type); //滑动列表事件

	string getStarAttributeName(int n);

protected:
	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();
	void updateNextFram(float ft);


	StarsSuLayer();
private:

	StarsSuController *m_StarsSuController;

	STARSSY_UI  m_ui;

	CustomScrollBar *m_CustomScrollBar;
	ProgressTimer   *m_ProgressTimer;

	ASTROLOGY_TYPE  m_SendType;

	int  m_coinSpendCount;

	bool   m_isPushScene;

	string StarMaleAttributeName[11];

	string StarFeMalAttributeName[11];
};




#endif // !StarsSuLayer_h_
