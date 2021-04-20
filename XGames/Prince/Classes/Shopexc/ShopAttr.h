/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ShopAttr
 *  Description:	通用商店属性，用以实现不同种类商店的操作封装;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/11/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef ShopAttr_h__
#define ShopAttr_h__

#include "ShopexcDef.h"
#include "DataManager/DataManager.h"

class ShopAttr
{
public:
	ShopAttr(void) { m_shopType = SHOP_TYPE_NONE; };
	virtual ~ShopAttr(void) {};

	// 设置/获取货币接口;
	virtual void  setMoney(int nMoney) = 0;
	virtual int   getMoney() = 0;

	// 限购条件(除货币总量等通用条件外，各类型商店的个性化条件判定);
	virtual bool  isItemValid(const ShopexcItemParam& param) = 0;

	// 获取VIP刷新次数上限字段名;
	virtual string  getMaxRefreshTimesFieldByVip() = 0;

	// 更新已刷新次数;
	virtual int   getRefreshTimes() = 0;
	virtual void  updateRefreshTimes() = 0;

	// 检查刷新时需要消耗的货币数量是否足够;
	virtual bool  checkRefreshCost()
	{
		return getRefreshCost() <= getMoney();
	}

	// 获取各商店刷新价格;
	virtual int   getRefreshCost()
	{
		int nCost = 0;

		// 从系统表读取每次价格;
		int nId = getSysId();

		if (-1 != nId)
		{
			RowData* _data = DataManager::getInstance()->getDTToolSystem()->searchDataById(nId);
			assert(_data != nullptr);
			string str = _data->getStringData("value");
			vector<int>  vcPrice;
			StringFormat::parseCsvStringVec(str, vcPrice);
			assert(vcPrice.size() > 0);
			int nIndex = getRefreshTimes() + 1;

			if (nIndex > vcPrice.size())
				nIndex = vcPrice.size();

			nCost = vcPrice.at(nIndex-1);
		}

		return nCost;
	};

protected:

	// 获取各商店在system表里的Id;
	virtual int  getSysId() = 0;

protected:

	ShopType  m_shopType;
};

#endif // ShopAttr_h__
