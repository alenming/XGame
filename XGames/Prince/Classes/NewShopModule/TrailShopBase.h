/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		TrailShopBase
 *  Description:	试练商店;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef TrailShopBase_h__
#define TrailShopBase_h__

#include "ShopBase.h"

class TrailShopBase : public ShopBase
{
public:
	TrailShopBase(void);
	~TrailShopBase(void);

	virtual int   getMoney();
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif // TrailShopAttr_h__
