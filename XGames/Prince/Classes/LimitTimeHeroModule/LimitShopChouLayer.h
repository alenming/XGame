/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopChouLayer
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef LimitShopChouLayer_h_
#define LimitShopChouLayer_h_

#include "LimitTimeHeroDataDef.h"
#include "LimitTimeHeroController.h"
#include "LimitTimeHeroModel.h"
#include "ToolModule/UIToolHeader.h"

struct Limit_OneChou
{
	Layout	*Pnl_Item;
	Text	*Lab_Item;

};

struct Limit_CHOU
{
	Layout		*ui_Root;		//
	Layout		*Panel_38;

	Button		*Btn_Once;
	ImageView	*Img_Gold;
	ImageView	*Img_Free;
	Text		*Lab_GoldCost;

	Button		*Btn_Tenth;
	ImageView	*Img_GoldT;
	ImageView	*Img_FreeT;
	Text		*Lab_GoldCostT;

	Layout*   Pnl_Tenth;
	Layout*   Pnl_Once;

	Limit_OneChou	oneChou;
	std::vector<Limit_OneChou> tenChou;
};


class LimitShopChouLayer:public Layer,public Observer
{
public:
	~LimitShopChouLayer();

	//CREATE_FUNC(ShopChouLayer);
	static LimitShopChouLayer* create(eChouKaType type);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

protected:
	LimitShopChouLayer();

	virtual bool init(eChouKaType type);
	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void update(float dt);

	void initUi();

	void updatePublicInfo();

	void updateOneChou();

	void updateTenChou();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type,eChouKaType taketype);

	void playingOneItemAnimation(UIToolHeader* item,int index);

	void updateNextFram(float ft);
private:
	CC_SYNTHESIZE(bool,_isFirstRun,FirstRun);

	Limit_CHOU	m_ui;

	eChouKaType m_type;

	bool firstChou;

	bool isRunend;

};

#endif //LimitShopChouLayer
