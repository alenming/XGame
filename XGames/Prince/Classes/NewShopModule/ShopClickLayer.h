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
#ifndef ShopClickLayer_h_
#define ShopClickLayer_h_

#include "ShopDef.h"
#include "ShopController.h"
#include "ShopModel.h"
#include "LayerShopOne.h"

enum ONEITEM_CB
{
	ONE_ITEM_EXIT,
	ONE_ITEM_BUY
};

class ShopClickLayer:public ModalLayer 
{
public:
	~ShopClickLayer();

	static ShopClickLayer* create(ShopItemParam index,LayerShopOne* la);

	virtual bool init(ShopItemParam index, LayerShopOne* la);
	virtual void onEnter() override;
	virtual void onExit() override;

	ShopClickLayer();
	Layout* getSon();
	ImageView* getImagviewDi();
protected:
	
	void initUi();

	void updataInfo();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, ONEITEM_CB _callback);

	std::string checkItemScoreImg(ShopItemParam Param);


private:

	UIOneItem m_ui;
	 
	ShopItemParam index;
	LayerShopOne* parrent;

};

#endif //ShopLayer_h_
