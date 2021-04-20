/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *  FileName:		DoubleBonusData
 *  Description:	双倍奖励活动各模块统一数据结构
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			06/14/2016
 *  Others: 
 ******************************************************************/
#ifndef DoubleBonusData_h__
#define DoubleBonusData_h__

#include "DoubleBonusDef.h"
#include "DoubleBonusListener.h"

class DoubleBonusData
{
public:
	DoubleBonusData(void);
	~DoubleBonusData(void);

	// 查询活动当前状态;
	DOUBLE_BONUS_STATE  getState();

	// 活动当前是否可用;
	bool  isValid();

	// listener回调;
	void  onTimeOut();

	// 计时相关(一个活动绑定一个listener实例);
	bool  startListener();

private:

	void  resetListener();

public:

	int					nModuleId;		// 模块Id
	DOUBLE_BONUS_TYPE	nType;			// 类型;
	int					nStartStamp;	// 开始时间;
	int					nEndStamp;		// 结束时间;

	// 以下是保留参数(用以扩充已有的参数类型，以及兼容不同模块间同类型但不同含义的参数);
	int					nReservedParam1;
	int					nReservedParam2;
	string				strReservedParam1;

	// listener实例;
	DoubleBonusListener* listener;

	// 上一个状态;
	DOUBLE_BONUS_STATE   preState;
};

#endif // DoubleBonusData_h__

