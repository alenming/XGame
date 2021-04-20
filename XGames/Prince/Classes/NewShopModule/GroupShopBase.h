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
#ifndef GroupShopBase_h__
#define GroupShopBase_h__

#include "ShopBase.h"

class GroupShopBase : public ShopBase
{
public:
	GroupShopBase(void);
	~GroupShopBase(void);

	virtual int   getMoney();
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif //GroupShopBase_h__
