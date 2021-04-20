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
#ifndef RefineShopAttr_h__
#define RefineShopAttr_h__

#include "ShopAttr.h"
class RefineShopAttr : public ShopAttr
{
public:
	RefineShopAttr(void);
	~RefineShopAttr(void);

	virtual void  setMoney(int nMoney);
	virtual int   getMoney();
	virtual bool  isItemValid(const ShopexcItemParam& param);
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif // RefineShopAttr_h__
