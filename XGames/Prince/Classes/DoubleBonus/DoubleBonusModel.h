/******************************************************************* 
 *  Copyright(c) 2016 zcjoy  All rights reserved. 
 *
 *  FileName:		DoubleBonusModel
 *  Description:	双倍奖励活动
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

	// 解析服务器数据;
	void  parseDoubleBonusData(const Json::Value &data);

	// 查询活动是否在当前可用;
	bool  isDoubleBonusValid(DOUBLE_BONUS_TYPE type);

	// 获取一个活动的配置;
	const vector<DoubleBonusData*>* getDataByType(DOUBLE_BONUS_TYPE type);

	// 活动时间节点通知;
	static void  onTimeOut(DOUBLE_BONUS_LISTENER_TIME_OUT_TYPE type, void* data);

private:
	DoubleBonusModel(void);

	// 类型映射;
	DOUBLE_BONUS_TYPE  getTypeByInt(int nVal);

	// 检查所有活动时间，然后分类并执行计时;
	void  checkAndRun();

private:
	static DoubleBonusModel* m_pInstance;

	// 双倍活动原始数据;
	map<DOUBLE_BONUS_TYPE, vector<DoubleBonusData*> >  m_Data;
};

#endif // DoubleBonusModel_h__

