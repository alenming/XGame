/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *  FileName:		DoubleBonusData
 *  Description:	˫���������ģ��ͳһ���ݽṹ
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

	// ��ѯ���ǰ״̬;
	DOUBLE_BONUS_STATE  getState();

	// ���ǰ�Ƿ����;
	bool  isValid();

	// listener�ص�;
	void  onTimeOut();

	// ��ʱ���(һ�����һ��listenerʵ��);
	bool  startListener();

private:

	void  resetListener();

public:

	int					nModuleId;		// ģ��Id
	DOUBLE_BONUS_TYPE	nType;			// ����;
	int					nStartStamp;	// ��ʼʱ��;
	int					nEndStamp;		// ����ʱ��;

	// �����Ǳ�������(�����������еĲ������ͣ��Լ����ݲ�ͬģ���ͬ���͵���ͬ����Ĳ���);
	int					nReservedParam1;
	int					nReservedParam2;
	string				strReservedParam1;

	// listenerʵ��;
	DoubleBonusListener* listener;

	// ��һ��״̬;
	DOUBLE_BONUS_STATE   preState;
};

#endif // DoubleBonusData_h__

