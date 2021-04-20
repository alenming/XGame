/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopModel
 *  Description:	集市模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ShopModel_h__
#define ShopModel_h__

#include "ShopDef.h"
#include "Utils/Observer.h"
#include "ShopBase.h"

enum COMMONSHOP_KEY
{
	FREECHOUTIME  = 10087,
	HAVE_KEY_COUTN = 10088

};  

class ShopModel : public Observable,public TimeStampListener, public TimeLocalListener
{
public:
	ShopModel(void);
	~ShopModel(void);

	static ShopModel*  getInstance();
	static void  destroyInstance();

	//计时通知 param: 时间间隔
	virtual void timeOut();

	//计时通知 param:刷新秒数
	virtual void timeTick(int dt);

	void setLocalTime();

	void setEndTime(int timestate);

	// 查询/设置当前商店模式;
	void setShopType(SHOP_TYPE type);
	SHOP_TYPE getShopType();

	// 更新公共信息;
	void updatePublicInfo();
	//更新初始界面信息
	void updateFirstShopInfo();
	// 更新商品内容<Id, <Count, Price> >;
	void  updateAllItems(vector<ShopItemParam> vcAllItems);

	void updateCuilian();
	 
	void initCuiLian();

	// 查询商品内容;
	ShopItemParam*  getItem(int nSrvPos);

	// 设置/获取刷新次数;
	int   getMaxRefreshCount();
	void  updateRefreshCount();
	int   getLeftRefreshCount();

	// 获取刷新消耗;
	int   getRefreshCost();

	// 查询商品是否可购买;
	int   isItemValid(int nIndex);

	// 商品购买成功;
	void  onItemSold(int nSrvPos, int flag);

	// 检查价格状态;
	bool  checkPrice(int nItemIndex);

	// 获取/设置宿主货币总量;
	int   getMoney();
	//void  setMoney(int nMoney);

	// 检查刷新时需要消耗的货币数量是否足够;
	bool  checkRefreshCost();
	//更新抽完之后的展示信息
	void updateChouInfo(CHOU_TYPE type);

	//FirstShop属性
	CC_SYNTHESIZE(int, m_freshCount,FreshCount);  //刷新令牌数量
	CC_SYNTHESIZE(bool, m_isFree, IsFree);			//是否是免费抽
	std::vector<ChouItemParam> m_canGetPram;  //可能获得的东西
	std::vector<ChouItemParam> m_ChouPram;  //抽到的获得的东西
	CC_SYNTHESIZE(int, m_oneChou,OneChouPrice);		//抽一次的价格
	CC_SYNTHESIZE(int, m_tenChou,TenChouPrice);		//抽十次的价格
	CC_SYNTHESIZE(bool, m_isPushScene, IsPushScene);	//是否是PushScene
	CC_SYNTHESIZE(bool, m_SuccessFul, ChouSuccessful);
	CC_SYNTHESIZE(bool, m_freshSuc, FreshSuccessful);
	CC_SYNTHESIZE(bool, m_firstChou, FirstChou);
	CC_SYNTHESIZE(bool, m_isShowFlage,IsShowFlag);
	CC_SYNTHESIZE(int, m_freechouTime, FreeChouTime); //到计时抽时间
	CC_SYNTHESIZE(int ,m_nStamp, Stamp);			//商店刷新倒计时
	vector<ShopItemParam>& getCuiLianInfo()
	{
		return vcGoods;
	}

	vector<ShopItemParam>* getm_vcAllItems()
	{
		return &m_vcAllItems;
	}

	Layout *m_root;
private:

	void  initAttr(SHOP_TYPE shopType);

	void  updateUI(SHOP_EVENT_TYEP type, void* data);

	// 重置倒计时;
	void  resetCountDown(int nSec = 0);

private:
	static ShopModel* m_pInstance;

	// 当前商店类型;
	SHOP_TYPE  m_shopType;

	// 具体商店属性;
	ShopBase* m_pAttr;

	// 当前所有商品信息;
	vector<ShopItemParam>  m_vcAllItems;

	vector<ShopItemParam>  vcGoods;//淬练商店信息//本地表

	// 当前刷新次数上限和已刷新次数;
	int  m_nMaxRefreshCount;
	int  m_nRefreshCount;
};

#endif // ShopModel_h__
