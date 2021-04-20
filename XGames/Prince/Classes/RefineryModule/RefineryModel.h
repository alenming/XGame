/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		RefineryModel
 *  Description:	Refinery数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/22/2015
 *  Others:			
 *
 ******************************************************************/
#include "RefineryHeader.h"


#ifndef RefineryModel_h_
#define RefineryModel_h_

class RefineryModel:public Observable
{
public:
	~RefineryModel();

	//活动单例
	static RefineryModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(REFINERY_EVENT_TYPE eventType, void* data = nullptr);

	void parseRefineryData(const Json::Value &data); //解析炼化炉数据

	REFINERY_STATE& getRefineryState() {return m_state;};

private:
	RefineryModel();

	static RefineryModel* mInstance;

	REFINERY_STATE m_state;
};


#endif