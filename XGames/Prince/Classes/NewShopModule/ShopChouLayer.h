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
#ifndef ShopChouLayer_h_
#define ShopChouLayer_h_

#include "ShopDef.h"
#include "ShopController.h"
#include "ShopModel.h"
#include "Widget/LayerCommHeadInfo.h"


class ShopChouLayer:public Layer,public Observer
{
public:
	~ShopChouLayer();

	//CREATE_FUNC(ShopChouLayer);
	static ShopChouLayer* create(CHOU_TYPE type);
	 
	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data);

protected:
	ShopChouLayer();

	virtual bool init( CHOU_TYPE type);
	virtual void onEnter() override;
	virtual void onExit() override;

	void initUi();

	void updatePublicInfo();

	void updateOneChou();

	void updateTenChou();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type,CHOU_TYPE chouType);

	void playingOneItemAnimation(UIToolHeader* item,int index);

	void updateNextFram(float ft);
private:
	CC_SYNTHESIZE(bool,_isFirstRun,FirstRun);

	UI_CHOU	m_ui;

	CHOU_TYPE m_type;
	 
	std::vector<UIToolHeader*> m_Tool;

	bool firstChou;

	bool isRun;

};

#endif //ShopChouLayer_h_
