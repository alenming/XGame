/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		DoubleBonusDef
 *  Description:	双倍奖励活动数据定义
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			06/13/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef DoubleBonusDef_h__
#define DoubleBonusDef_h__

#include "Utils/LocalTimer.h"
#include "Utils/StringFormat.h"

enum DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE
{
	START = 1001,			// 活动开启;
	END   = 1002			// 活动结束;
};

enum DOUBLE_BONUS_TYPE
{
	DOUBLE_BONUS_TYPE_NONE,					// 未知类型;
	DOUBLE_BONUS_TYPE_STORY,				// 普通关卡;
	DOUBLE_BONUS_TYPE_ELITE,				// 精英关卡;
	DOUBLE_BONUS_TYPE_ACTIVITY_CHIP,		// 活动关卡-江湖传闻;
	DOUBLE_BONUS_TYPE_ACTIVITY_EXP,			// 活动关卡-酒经考验;
	DOUBLE_BONUS_TYPE_ACTIVITY_COIN,		// 活动关卡-梁山秘藏;
	DOUBLE_BONUS_TYPE_BUY_COIN,				// 购买-银币(祈福);
	DOUBLE_BONUS_TYPE_BUY_POWER				// 购买-体力;
};

enum DOUBLE_BONUS_STATE
{
	DOUBLE_BONUS_STATE_INVALID,				// 不可用(因为服务器下发列表会过滤掉短期内不上的活动，所以此处主要指过期的);
	DOUBLE_BONUS_STATE_EXPECTED,			// 即将可用(跟服务器约定指的是2天内会上的活动);
	DOUBLE_BONUS_STATE_VALID				// 当前可用;
};

#endif // DoubleBonusDef_h__