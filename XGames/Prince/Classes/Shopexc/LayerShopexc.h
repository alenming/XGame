/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerShopexc
 *  Description:	通用的积分商店基类
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			05/28/2015
 *  Others:			08/27/2015  属性可继承扩展  v1.1 by Phil
 *
 ******************************************************************/
#ifndef LayerShopexc_h__
#define LayerShopexc_h__

#include "ShopexcDef.h"
#include "Utils/Observer.h"
#include "ShopexcController.h"
#include "ShopexcModel.h"

class LayerShopexc;
class ShopexcItem : public Layout
{
public:
	~ShopexcItem();

	static ShopexcItem* create(ShopexcItemParam param, LayerShopexc* parent);

	virtual bool  init(ShopexcItemParam param, LayerShopexc* parent);

	// 更新在售状态;
	void  updateState(bool bEnabled);

	// 更新价格状态;
	void  updatePrice();

private:
	ShopexcItem();

	void  initUI(ShopexcItemParam param);

	void  onBtnClick(Ref* ref, Widget::TouchEventType type);

private:
	UIShopexcItem  m_ui;

	LayerShopexc*  m_parent;

	ShopexcItemParam _param;

	int  m_nIndex;
};


class LayerShopexc : public Layer, public Observer
{
public:
	~LayerShopexc(void);

	static LayerShopexc* create(ShopType type);

	virtual void  onEnter();

	virtual bool  init(ShopType type);

	void  onBtnClick(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  onScrollViewEvent(Ref* pSender, ScrollView::EventType type);

	void  onItemClicked(int nIndex);

	virtual void  update(float delta);

	virtual void  updateSelf(void* pData);

	static string checkScoreImg(ShopType type);

	static string checkItemScoreImg(ShopexcItemParam Param);

	ImageView* getImageViewBg();

private:
	LayerShopexc(void);

	void  initUI();

	// 更新所有物品;
	void  refreshAllItem(const vector<ShopexcItemParam>* vcAllItem);

	// 更新单个物品(目前只是更新状态);
	void  refreshSingleItem(const ShopexcItemParam* item);

	// 刷新按钮;
	void  updateRefreshCount(int nCount);

	// 图片切换;
	string			checkTitleImg();
	void			checkRefreshBtn();

	// 类型名称(0-刷新 1-购买，针对八卦/九宫商店做的区分，其他商店都是刷新);
	string  getStrTypeName(int opType = 0);

	// 进场动画;
	void    playEnterAnimation();

private:
	ShopType  m_type;

	UIShopexc  m_ui;

	ShopexcController*  m_pController;

	vector<ShopexcItem*>  m_vcAllItems;

	bool  m_bFirstEnter;



	enum Shopexc_Widget
	{
		Btn_Close,
		Lab_Score,
		Btn_Refresh,
		Lab_Time
	};
};

#endif // LayerShopexc_h__
