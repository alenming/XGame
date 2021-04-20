/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampLayer
 *  Description:	星盘层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef StarsCampLayerHero_h_
#define StarsCampLayerHero_h_

#include "StarsCampDef.h"
#include "StarsCampModel.h"
#include "UI/ModalLayer.h"

enum SC_HERO_CALLBACK
{
	SC_HERO_EXIT,
	SC_HERO_SHOPORXX
};

class StarsCampLayerHero:public ModalLayer
{
public:
	~StarsCampLayerHero();

	CREATE_FUNC(StarsCampLayerHero);
	 
	ImageView* getSonChirld();

	virtual bool init() ;
	virtual void onEnter() ;
	virtual void onExit() ;
protected:
	StarsCampLayerHero();

	void initUi();

	void updateUI();
	void updateFourAttrubite(bool isston);
	void updateCollect();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type,SC_HERO_CALLBACK tyep);

private:
	 UI_JumpLayer m_ui;
};

#endif // !StarsCampNewLayer_h_
