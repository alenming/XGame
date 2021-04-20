/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		TrailShopAttr
 *  Description:	试炼商店属性的实现;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/11/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef TrailShopAttr_h__
#define TrailShopAttr_h__

#include "ShopAttr.h"
class TrailShopAttr : public ShopAttr
{
public:
	TrailShopAttr(void);
	~TrailShopAttr(void);

	virtual void  setMoney(int nMoney);
	virtual int   getMoney();
	virtual bool  isItemValid(const ShopexcItemParam& param);
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif // TrailShopAttr_h__
