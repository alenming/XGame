/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LimitTimeHeroController.h
 *  Description:	��ʱ��ģ���Controllerʵ��;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			04/11/2016
 *  Others:			
 *
 ******************************************************************/
#ifndef LimitTimeHeroController_h__
#define LimitTimeHeroController_h__

#include "Communication/ICommListener.h"
#include "LimitTimeHeroDataDef.h"

class LimitTimeHeroController : public ICommListener
{
public:
	LimitTimeHeroController();
	~LimitTimeHeroController();

public:

	//��ȡ��������
	static void sendGetTableDataMsg();

	//��ȡ�����������
	static void sendGetMainInfoMsg();

	//�鿨/��ϻ param.1.�鿨/������; 2.һ��/ʮ��
	static void sendTakeCardMsg(eActType actType, eChouKaType type);

	//��ȡ���ֶα��佱�� param. ����
	static void sendGetScoreRewardMsg(int boxId);

private:

	void parseTableDataMsg(const char* szData);

	void parseMainInfoMsg(const char* szData);

	void parseTakeCardMsg(const char* szData);

	void parseScoreRewardMsg(const char* szData);

private:
	virtual void onRecvMessage(int nCmdID, const char* szData);
};

#endif