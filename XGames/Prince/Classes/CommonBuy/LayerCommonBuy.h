/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerCommonBuy
 *  Description:	通用购买界面;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			08/10/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef LayerCommonBuy_h__
#define LayerCommonBuy_h__

#include "CommonBuyDef.h"
#include "Utils/Observer.h"
#include "CommonBuyController.h"
#include "DoubleBonus/DoubleBonusDef.h"

class CommonBuyController;
class LayerCommonBuy : public Layer, public Observer
{
public:
	~LayerCommonBuy(void);

	static LayerCommonBuy* create(COMMON_BUY_TYPE type);

	virtual bool  init(COMMON_BUY_TYPE type);

	virtual void  onEnter();

	virtual void  updateSelf(void* data);

	void  update(float delta);

	void  onBtnClicked(Ref* ref, Widget::TouchEventType type, int nWidgetName);

	void  updateUI(const CommonBuyData  data);

	void  updateMultiBuy();

	int  geteMultiBuySpendGold(CommonBuyData const data);

private:

	// 获取当前VIP级别图标;
	string  getVipIcon(int nVipLv);

	// 检测是否能购买(当前次数/可提升次数);
	bool  checkBuyCondition();

	void  showDoubleBonusTips(bool bShow);

private:
	LayerCommonBuy(void);

	void  initUI();

private:

	UI_Common_Buy  m_ui;

	// 购买类型;
	COMMON_BUY_TYPE  m_type;

	CommonBuyData  m_data;

	CommonBuyController*  m_pController;

	// 首次进入;
	bool  m_bFirstEnter;

	int  m_buyCount;
	int  m_buyMutiCount;
	int  m_buySpend;
	int  m_MutiBuyCoinCostGoldCount;

	enum UI_COMMONBUY_WIDGET
	{
		Btn_Close,
		Btn_OK,
		Btn_Cancel,
		Btn_Buy,
		Btn_Multi_Buy,

		Btn_Multi_OK,
		Btn_Multi_Cancel,
		Btn_Multi_Close,

		Btn_Vip_OK,
		Btn_Vip_Cancel,
		Btn_Vip_Close
	};

};

#endif // LayerCommonBuy_h__
