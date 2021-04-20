/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopexcModel
 *  Description:	商店数据;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			05/28/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef ShopexcModel_h__
#define ShopexcModel_h__

#include "ShopexcDef.h"
#include "Utils/Observer.h"
#include "ShopAttr.h"

class ShopexcModel : public Observable
{
public:
	ShopexcModel(void);
	~ShopexcModel(void);

	static ShopexcModel*  getInstance();
	static void  destroyInstance();

	// 查询/设置当前商店模式;
	void  setShopType(ShopType type);
	ShopType getShopType();

	// 更新公共信息;
	void  updatePublicInfo();

	// 更新商品内容<Id, <Count, Price> >;
	void  updateAllItems(vector<ShopexcItemParam> vcAllItems);

	// 查询商品内容;
	ShopexcItemParam*  getItem(int nSrvPos);

	// 设置/获取刷新次数;
	int   getMaxRefreshCount();
	void  updateRefreshCount();
	int   getLeftRefreshCount();

	// 获取刷新消耗;
	int   getRefreshCost();

	// 查询商品是否可购买;
	int   isItemValid(int nIndex);

	// 商品购买成功;
	void  onItemSold(int nSrvPos, int nLeftMoney);

	// 倒计时;
	void  onCountDown(int nSec);

	// 检查价格状态;
	bool  checkPrice(int nItemIndex);

	// 获取/设置宿主货币总量;
	int   getMoney();
	void  setMoney(int nMoney);

	// 检查刷新时需要消耗的货币数量是否足够;
	bool  checkRefreshCost();

	CC_SYNTHESIZE(bool, m_SuccessFul, ChouSuccessful);
private:

	void  initAttr(ShopType shopType);

	void  updateUI(OBS_PARAM_TYPE_SHOPESC type, void* data);

	// 重置倒计时;
	void  resetCountDown(int nSec = 0);

private:
	static ShopexcModel* m_pInstance;

	// 当前商店类型;
	ShopType  m_shopType;

	// 具体商店属性;
	ShopAttr* m_pAttr;

	// 当前所有商品信息;
	vector<ShopexcItemParam>  m_vcAllItems;

	// 当前刷新次数上限和已刷新次数;
	int  m_nMaxRefreshCount;
	int  m_nRefreshCount;

};

#endif // ShopexcModel_h__
