/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		MarvellousActiveModel
 *  Description:	精彩活动数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/13/2015
 *  Others:			
 *
 ******************************************************************/
#include "MarvellousActiveHeader.h"


#ifndef MarvellousActiveModel_h_
#define MarvellousActiveModel_h_

class MarvellousActiveModel:public Observable
{
public:
	~MarvellousActiveModel();

	//活动单例
	static MarvellousActiveModel* getInstance();
	static void destroyInstance();

	//通知监听者
	void notify(MA_EVENT_TYPE eventType, void* data = nullptr);

	void parseMaInfoData(const Json::Value &data); //解析界面进入拉取精彩活动数据

	MA_INFO_VEC& getMaInfo() {return m_MaInfo;};

private:

	static MarvellousActiveModel* mInstance;

	MarvellousActiveModel();


	MA_INFO_VEC  m_MaInfo; //精彩活动

	bool isAddLayerActivity; //是否每日战斗
};


#endif