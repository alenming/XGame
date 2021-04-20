/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		RefineShop9Attr
 *  Description:	����¯ - �Ź��̵����Ե�ʵ��;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			10/30/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef RefineShop9Attr_h__
#define RefineShop9Attr_h__

#include "ShopAttr.h"
class RefineShop9Attr : public ShopAttr
{
public:
	RefineShop9Attr(void);
	~RefineShop9Attr(void);

	virtual void  setMoney(int nMoney);
	virtual int   getMoney();
	virtual bool  isItemValid(const ShopexcItemParam& param);
	virtual int   getSysId();
	virtual string  getMaxRefreshTimesFieldByVip();
	virtual int   getRefreshTimes();
	virtual void  updateRefreshTimes();

	virtual int   getRefreshCost();

	virtual bool  checkRefreshCost();
};

#endif // RefineShop9Attr_h__
