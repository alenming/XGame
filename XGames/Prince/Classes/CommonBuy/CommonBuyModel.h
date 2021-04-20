/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		CommonBuyModel
 *  Description:	通用购买数据模型;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			09/15/2015
 *  Others: 
 *
 ******************************************************************/
#ifndef CommonBuyModel_h__
#define CommonBuyModel_h__

#include "CommonBuyDef.h"
#include "Utils/Observer.h"
#include "DoubleBonus/DoubleBonusDef.h"

class CommonBuyModel : public Observable
{
public:
	~CommonBuyModel(void);

	static CommonBuyModel* getInstance();
	static void  destroyInstance();

	// 查询和更新当前类型的已购买次数;
	int   getTimes();
	void  setTimes(int nTimes);

	// 查询当前最大可购买次数;
	int   getCurMaxTimes();

	// 类型字段名;
	string  getTypeField();

	// 查询下一个可用的VIP等级(购买次数有提升);
	int   getNextValidVip(int& nNextValue);

	// 查询多次祈福的次数;
	int   getMultiTimes();

	// 更新UI;
	void  updateData(COMMON_BUY_TYPE type);

	// 数据表项;
	void  getSysId(int& nPriceId, int& nAmountId);

	// 查询单价;
	int   getPrice();

	// 查询单次购买数量;
	int   getBuyAmount();

	// 双倍活动相关;
	bool  isDoubleValidByBuyTimes();
	COMMON_BUY_TYPE  checkTypeByDoubleBonusType(DOUBLE_BONUS_TYPE type);
	DOUBLE_BONUS_TYPE  checkTypeByBuyType(COMMON_BUY_TYPE buyType);

private:
	CommonBuyModel(void);

private:
	static CommonBuyModel* m_pInstance;

	COMMON_BUY_TYPE  m_type;
};

#endif // CommonBuyModel_h__
