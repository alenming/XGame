/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		CuilianShopBase
 *  Description:	PVP商店;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			03/04/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef CuilianShopBase_h__
#define CuilianShopBase_h__

#include "ShopBase.h"

class CuilianShopBase : public ShopBase
{
public:
	CuilianShopBase(void);
	~CuilianShopBase(void);

	virtual int   getMoney();

	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();
};

#endif //CuilianShopBase_h__
