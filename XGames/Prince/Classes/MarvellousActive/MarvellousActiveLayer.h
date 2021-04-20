/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		MarvellousActiveLayer
 *  Description:	精彩活动层模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/13/2015
 *  Others:			
 *
 ******************************************************************/
#include "MarvellousActiveHeader.h"
#include "MarvellousActiveController.h"
#include "MarvellousActiveModel.h"
#include "ChapterModule/Activity/LayerActivity.h"

#ifndef MarvellousActiveLayer_h_
#define MarvellousActiveLayer_h_

class MarvellousActiveLayer:public Layer,public Observer
{
public:
	~MarvellousActiveLayer();

	CREATE_FUNC(MarvellousActiveLayer);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

protected:
	MarvellousActiveLayer();

	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();

	void onScrollViewEvent( Ref* pSender, cocos2d::ui::ScrollView::EventType type);

	void setListView();
	bool openLevel(int id,int &lv);
	void setIcon(int id,ImageView *title,ImageView *icon,ImageView *open);

	void setListTime(int id);
	void updateCountDown(float ft);
private:
	UI_MA_ENTER m_ui;

	bool  isfirstGetInfo;
	bool   m_isAddScrollViewEvent;

	MarvellousActiveController mMarvellousActiveController;
};

#endif // MarvellousActiveLayer_h_
