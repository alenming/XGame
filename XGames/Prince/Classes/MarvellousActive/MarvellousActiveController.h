/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		MarvellousActiveController
 *  Description:	精彩活动控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/13/2015
 *  Others:			
 *
 ******************************************************************/
#include "MarvellousActiveHeader.h"
#include "MarvellousActiveModel.h"

#ifndef MarvellousActiveController_h_
#define MarvellousActiveController_h_

class MarvellousActiveController:public ICommListener
{

public:

	MarvellousActiveController();
	~MarvellousActiveController();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData) override;

	//界面拉取精彩活动数据
	static void getMaInfo();

	//解析精彩活动数据
	void processMaInfoData(const char* szData); 
	
};


#endif