/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		PvpShopBase
 *  Description:	PVP商店;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef PvpShopBase_h__
#define PvpShopBase_h__

#include "ShopBase.h"

class PvpShopBase : public ShopBase
{
public:
	PvpShopBase(void);
	~PvpShopBase(void);

	virtual int   getMoney();
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif //PvpShopBase_h__
