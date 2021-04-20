/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsSuScene
 *  Description:	星宿控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/10/2015
 *  Others:			
 *
 ******************************************************************/
#include "StarsSuHeader.h"
#include "StarsSuModel.h"

#ifndef StarsSuController_h_
#define StarsSuController_h_

class StarsSuController:public ICommListener
{
public:
	StarsSuController();
	~StarsSuController();

	virtual void onRecvMessage(int nCmdID, const char* szData) override;
	void processGetStarInfoData(const char* szData); //解析拉取反馈信息
	void processonAstrologyAndBreakOut(const char* szData); //解析占星反馈信息 解析突破反馈信息


	void onGetStarsInfo(); //拉取星宿界面信息
	void onAstrologyAndBreakOut(ASTROLOGY_TYPE type); //占星或者突破（消息格式一样 所以公用一条消息）
};


#endif