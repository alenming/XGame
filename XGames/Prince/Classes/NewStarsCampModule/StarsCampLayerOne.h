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

#ifndef StarsCampNewOneCampLayer_h_
#define StarsCampNewOneCampLayer_h_

#include "StarsCampDef.h"
#include "StarsCampController.h"
#include "StarsCampModel.h"
#include "Widget/LayerCommHeadInfo.h"
#include "StarsCampLayerHero.h"

enum ONECAMP_CALLBACK
{
	ONECAMP_CLECKHERO,
};

class StarsCampLayerOne:public Layer ,public Observer
{
public:
	~StarsCampLayerOne();

	CREATE_FUNC(StarsCampLayerOne);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

protected:
	StarsCampLayerOne();

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();

	void updateLeft();
	void updateRight();
	void updateMid();

	void initLeft();
	void initRight();
	void initMid();
	//回调汇总
	void onBtnClick( Ref* reff, Widget::TouchEventType type,ONECAMP_CALLBACK TYEP,int heroId, Button* son);

	void jumpToOnHeroInfo();

	void setJoinInFlag(Button* icon, bool isShow);

	void setAllFlag();
private:
	UI_StarsCamp_OneCamp m_ui;
	StarsCampLayerHero *m_layer;
};




#endif // !StarsCampNewLayer_h_
