/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		PvpShopAttr
 *  Description:	PvP商店属性的实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/11/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef PvpShopAttr_h__
#define PvpShopAttr_h__

#include "ShopAttr.h"

class PvpShopAttr : public ShopAttr
{
public:
	PvpShopAttr(void);
	~PvpShopAttr(void);

	virtual void  setMoney(int nMoney);
	virtual int   getMoney();
	virtual bool  isItemValid(const ShopexcItemParam& param);
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif // PvpShopAttr_h__
