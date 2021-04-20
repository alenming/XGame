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

#include "ShopexcDef.h"
#include "ShopexcController.h"
#include "ShopexcModel.h"
#include "UI/ModalLayer.h"
#include "LayerShopexc.h"

enum ONEITEM_CBEXC
{
	ONE_ITEM_EXIT,
	ONE_ITEM_BUY
};

//一个商品的界面
typedef struct tagUI_OneItem
{
	Layout *ui_Root;		//根结点
	Layout *Pnl_Item;		//商品头像
	ImageView	*Img_di1;
	Button*		Btn_Close;
	Button*		Btn_Buy;

	Text*		Lab_ItemName;//商品名称
	Text*		Lab_Count;//已经拥有的数量
	Text*		Lab_Tips;//商品说明
	Text*		Lab_Cost;//货币数量
	ImageView	*Img_CosIcon;//货币图片 

}UIexcOneItem;

class ShopexcClickLayer:public ModalLayer 
{
public:
	~ShopexcClickLayer();

	static ShopexcClickLayer* create(ShopexcItemParam index,LayerShopexc* la);

	virtual bool init(ShopexcItemParam index, LayerShopexc* la);
	virtual void onEnter() override;
	virtual void onExit() override;

	ShopexcClickLayer();
	Layout* getSon();
	ImageView* getImagviewDi();
protected:
	
	void initUi();

	void updataInfo();
	//回调汇总
	void  onBtnClick( Ref* reff, Widget::TouchEventType type, ONEITEM_CBEXC _callback);

	std::string checkItemScoreImg(ShopexcItemParam Param);


private:

	UIexcOneItem m_ui;
	 
	ShopexcItemParam index;

	LayerShopexc* parrent;

};

#endif //ShopLayer_h_
