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

#include "NewShopModule/ShopDef.h"
#include "Utils/Observer.h"
#include "NewShopModule/ShopBase.h"

enum MYSTERYSHOP_KEY
{
	MYSTERY_SHOW  = 10089
}; 


class MysteryShopModel : public Observable,public TimeStampListener, public TimeLocalListener
{
public:
	MysteryShopModel(void);
	~MysteryShopModel(void);

	static MysteryShopModel*  getInstance();
	static void  destroyInstance();

	//计时通知 param: 时间间隔
	virtual void timeOut();

	//计时通知 param:刷新秒数
	virtual void timeTick(int dt);

	void setLocalTime();

	void setEndTime( );

	void updateUI( SHOP_EVENT_TYEP type, void* data );

	void updateAllItems( vector<ShopItemParam> vcAllItems );

	void updatePublicInfo();

	ShopItemParam* getItem(int index);

	//CC_SYNTHESIZE(int ,m_nStamp, Stamp);			//神秘商店倒计时
	void setStamp(int ty);
	int getStamp();
	CC_SYNTHESIZE(SHOP_TYPE, m_ShopType, ShopType);
	CC_SYNTHESIZE(bool, m_SuccessFul, ShopSuccessful);
	CC_SYNTHESIZE(bool, m_freshSuc, FreshSuccessful);
	CC_SYNTHESIZE(int, m_nDaoTime, DaoTime);
	CC_SYNTHESIZE(bool, m_ismainShow,IsMainShow);
	CC_SYNTHESIZE(bool, m_isiterLayerShow,IsInterShow);
	vector<ShopItemParam>* getm_vcAllItems()
	{
		return &m_vcAllItems;
	}
	int isItemValid( int nIndex );
	void onItemSold( int nSrvPos, int flag );

	int getMaxRefreshCount();

	int getRefreshCost();
	int getRefreshTimes();
	void updateRefreshCount(int dayCount);
	int getLeftRefreshCount();
	bool checkRefreshCost();
private:
	static MysteryShopModel* m_pInstance;

	// 当前商店类型;
	SHOP_TYPE  m_shopType;

	// 当前所有商品信息;
	vector<ShopItemParam>  m_vcAllItems;

	int m_nStamp;

};

#endif // ShopModel_h__
