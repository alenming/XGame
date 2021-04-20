/******************************************************************* 
 *  Copyright(c) 2015 zcjoy  All rights reserved. 
 *
 *  FileName:		ActivityController
 *  Description:	关卡模块控制器的实现;
 *					1. 服务器数据接收/发送接口;
 *					2. 本地csv表数据解析;
 *					3. 流程控制;
 *  Version:		1.0
 *  Author:			Phil
 *  Date:			12/21/2014
 *  Others: 
 *
 ******************************************************************/
#ifndef ActivityController_h__
#define ActivityController_h__

#include "Communication/ICommListener.h"
#include "Communication/Json/json.h"
#include "ActivityDef.h"

class ActivityController : public Ref, public ICommListener 
{
public:
	~ActivityController(void);

	static ActivityController* getInstance();
	static void destroyInstance();

	// 实现接收服务器消息接口;
	virtual void  onRecvMessage(int nCmdID, const char* szData);

	// 请求活动关卡状态数据;
	void  handleActivityState();

	//////////////////////////////////// 流程控制; //////////////////////////////////////

	// 增加活动挑战次数;
	void  handleBuyActivityTimes(int nActivityId, int nCount = 1);

	// 挑战活动关卡;
	int   handleActivityChallenge(int nActivityId, int nActivityBarrierId);

	// 扫荡;
	void  handleActivityBarrierSweep(int nActivityId, int nActivityBarrierId);

private:

	ActivityController(void);

	// 处理服务器返回的活动关卡进度数据;
	void  procActivityData(const char* szData);

	// 处理服务器返回的活动关卡挑战次数购买结果;
	void  procBuyActivityTimesResult(const char* szData);

	// 扫荡结果;
	void  procActivitySweepResult(const char* szData);

private:

	static ActivityController*  m_pInstance;
};

#endif // ActivityController_h__