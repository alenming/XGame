/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		DoubleBonusDef
 *  Description:	˫����������ݶ���
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
	START = 1001,			// �����;
	END   = 1002			// �����;
};

enum DOUBLE_BONUS_TYPE
{
	DOUBLE_BONUS_TYPE_NONE,					// δ֪����;
	DOUBLE_BONUS_TYPE_STORY,				// ��ͨ�ؿ�;
	DOUBLE_BONUS_TYPE_ELITE,				// ��Ӣ�ؿ�;
	DOUBLE_BONUS_TYPE_ACTIVITY_CHIP,		// ��ؿ�-��������;
	DOUBLE_BONUS_TYPE_ACTIVITY_EXP,			// ��ؿ�-�ƾ�����;
	DOUBLE_BONUS_TYPE_ACTIVITY_COIN,		// ��ؿ�-��ɽ�ز�;
	DOUBLE_BONUS_TYPE_BUY_COIN,				// ����-����(��);
	DOUBLE_BONUS_TYPE_BUY_POWER				// ����-����;
};

enum DOUBLE_BONUS_STATE
{
	DOUBLE_BONUS_STATE_INVALID,				// ������(��Ϊ�������·��б����˵������ڲ��ϵĻ�����Դ˴���Ҫָ���ڵ�);
	DOUBLE_BONUS_STATE_EXPECTED,			// ��������(��������Լ��ָ����2���ڻ��ϵĻ);
	DOUBLE_BONUS_STATE_VALID				// ��ǰ����;
};

#endif // DoubleBonusDef_h__