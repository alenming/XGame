/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		GroupShopBase
 *  Description:	PVP商店;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef ZahuoShopBase_h
#define ZahuoShopBase_h

#include "ShopBase.h"

class ZahuoShopBase : public ShopBase
{
public:
	ZahuoShopBase(void);
	~ZahuoShopBase(void);

	virtual int   getMoney();
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif //ZahuoShopBase_h
