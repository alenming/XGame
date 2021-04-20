/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		RefineryController
 *  Description:	炼化炉控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/22/2015
 *  Others:			
 *
 ******************************************************************/
#include "RefineryHeader.h"
#include "RefineryModel.h"

#ifndef RefineryController_h_
#define RefineryController_h_

class RefineryController:public ICommListener
{
public:

	RefineryController();
	~RefineryController();

public:

	virtual void onRecvMessage(int nCmdID, const char* szData) override; //接收消息反馈

	void processRefineryData(const char* szData);

	static void refiney(REFINERY_NODE_VEC materilVec);
};


#endif