/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopLayer
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ShopLayer_h_
#define ShopLayer_h_

#include "ShopDef.h"
#include "ShopController.h"
#include "ShopModel.h"
#include "DataManager/DataTable.h"
#include "Widget/LayerCommHeadInfo.h"
#include "LayerShopOne.h"


enum Shop_CallBack
{
	Shop_Box,
	Shop_Zahuo,
	Shop_Rongyu,
	Shop_Jifeng,
	Shop_Shengwang
};

class ShopLayer:public Layer,public Observer
{
public:
	~ShopLayer();

	static ShopLayer* create(SHOP_TYPE type, bool ispush);

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

	 //设置哪一个商店显示
	 void setWhichShopClieck(SHOP_TYPE type);
protected:
	ShopLayer();

	virtual bool init(SHOP_TYPE type, bool ispush);
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();

	void updateNextFram(float dt);

	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, SHOP_TYPE mBtnCallBackType);
	//抽一次十次回调
	void  onBtnChouClick( Ref* reff, Widget::TouchEventType type, CHOU_TYPE mBtnCallBackType);

	void onBtnClickByCuilian( Ref* reff, Widget::TouchEventType type);
	//接收数据后刷新免费抽
	void updateFirstShopUI();
	//更新宝匣界面
	void updateBaoJiaFace();

private:


private:

	UI_ShopLayer m_ui;

	LayerShopOne *_otherlayer;

	LayerShopOne *_cuiLianlayer;

	SHOP_TYPE	_shopType;

	SHOP_TYPE	_LastshopType;

	bool isFirst;
};

#endif //ShopLayer_h_
