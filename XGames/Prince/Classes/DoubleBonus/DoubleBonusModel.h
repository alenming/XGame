/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		DoubleBonusModel
 *  Description:	˫�������
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			06/12/2016
 *  Others: 
 *
 ******************************************************************/
#ifndef DoubleBonusModel_h__
#define DoubleBonusModel_h__

#include "Utils/Observer.h"
#include "Communication/Json/json.h"
#include "DoubleBonusDef.h"
#include "DoubleBonusData.h"

class DoubleBonusModel : public Observable
{
public:
	~DoubleBonusModel(void);

	static DoubleBonusModel* getInstance();
	static void destoryInstance();

	// ��������������;
	void  parseDoubleBonusData(const Json::Value &data);

	// ��ѯ��Ƿ��ڵ�ǰ����;
	bool  isDoubleBonusValid(DOUBLE_BONUS_TYPE type);

	// ��ȡһ���������;
	const vector<DoubleBonusData*>* getDataByType(DOUBLE_BONUS_TYPE type);

	// �ʱ��ڵ�֪ͨ;
	static void  onTimeOut(DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE type, void* data);

private:
	DoubleBonusModel(void);

	// ����ӳ��;
	DOUBLE_BONUS_TYPE  getTypeByInt(int nVal);

	// ������лʱ�䣬Ȼ����ಢִ�м�ʱ;
	void  checkAndRun();

private:
	static DoubleBonusModel* m_pInstance;

	// ˫���ԭʼ����;
	map<DOUBLE_BONUS_TYPE, vector<DoubleBonusData*> >  m_Data;
};

#endif // DoubleBonusModel_h__

