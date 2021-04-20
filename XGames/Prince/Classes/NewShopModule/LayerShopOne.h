/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerShopOne
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef LayerShopOne_h__
#define LayerShopOne_h__

#include "ShopDef.h"
#include "Utils/Observer.h"
#include "ShopController.h"
#include "ShopModel.h"

class LayerShopOne;
class ShopItem : public Layout
{
public:
	~ShopItem();

	static ShopItem* create(ShopItemParam param, LayerShopOne* parent);

	virtual bool  init(ShopItemParam param, LayerShopOne* parent);

	// 更新在售状态;
	void  updateState(bool bEnabled);

	// 更新价格状态;
	void  updatePrice();

private:
	ShopItem();

	void  initUI();

	void  onBtnClick(Ref* ref, Widget::TouchEventType type);

private:
	UIShopItem  m_ui;

	LayerShopOne*  m_parent;

	int  m_nIndex;

	ShopItemParam _param;

};


class LayerShopOne :  public Observer,public Layer
{
public:
	~LayerShopOne(void);

	static LayerShopOne* create(SHOP_TYPE type);

	virtual void  onEnter();

	virtual bool  init(SHOP_TYPE type);
	void  onBtnClick(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  onItemClicked(int nIndex);

	virtual void  update(float delta);

	virtual void  updateSelf(void* pData);

	static string checkScoreImg(SHOP_TYPE type);

	static string checkItemScoreImg(ShopItemParam Param);

	ImageView* getImageViewBg()
	{
		return m_ui.Img_Bg;
	}

private:
	LayerShopOne(void);



	// 更新所有物品;
	void  refreshAllItemOther();

	void  refreshAllItem();

	// 更新单个物品(目前只是更新状态);
	void  refreshSingleItem(const ShopItemParam* item);

	// 刷新按钮;
	void  updateRefreshCount(int nCount);

	// 图片切换;
	string			checkTitleImg();
	void			checkRefreshBtn();

	// 类型名称(0-刷新 1-购买，针对八卦/九宫商店做的区分，其他商店都是刷新);
	string  getStrTypeName(int opType = 0);

	// 进场动画;
	void    playEnterAnimation();

	void initUIByType();
	void initUINoCuilian();
	void  initUICuiLian();
private:
	SHOP_TYPE  m_type;

	UIShop m_ui; //淬练

	UI_Son m_Son;//不是淬练

	vector<ShopItem*>  m_vcAllItems;

	vector<ShopItem*>  m_vcAllItemsCuilian;

	bool  m_bFirstEnter;

	enum Shopexc_Widget
	{
		Btn_Close,
		Lab_Score,
		Btn_Refresh,
		Lab_Time
	};
};

#endif // LayerShopOne_h__
