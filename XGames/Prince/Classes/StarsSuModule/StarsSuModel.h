/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsSuScene
 *  Description:	星宿数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/10/2015
 *  Others:			
 *
 ******************************************************************/
#include "StarsSuHeader.h"


#ifndef StarsSuModel_h_
#define StarsSuModel_h_

class StarsSuModel:public Observable
{
public:
	~StarsSuModel();

	//活动单例
	static StarsSuModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(STARSSU_EVENT_TYPE eventType, void* data = nullptr);

	void parseGetStarInfoData(const Json::Value &val); //解析拉取星宿信息
	void parseAstrologyAndBreakOut(const Json::Value &val); //解析占星信息 解析突破信息

	const STARSTSU_INFO& getStarssSuInfo(); //获取星宿信息

	const int& getStatSuInfoCount()
	{
		return m_getStatSuInfoCount;
	}
private:
	StarsSuModel();

	static StarsSuModel* mInstance;

	STARSTSU_INFO  m_StarsSuInfo;
	int   m_getStatSuInfoCount;
};


#endif