/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		LayerSignHelp
 *  Description:	每日签到模块消息模块实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			06/15/2015
 *  Others:			
 *
 ******************************************************************/
#ifndef LayerSignController_h__
#define LayerSignController_h__

#include "Communication/ICommListener.h"
#include "Communication/SocketCommunicator.h"
#include "Communication/Command.h"
#include "LayerSignModel.h"

class LayerSignController :public ICommListener 
{
public:
	~LayerSignController();
	//活动单例
	static LayerSignController* getInstance();
	static void destroyInstance();

	virtual void onRecvMessage(int nCmdID, const char* szData);

	//发送拉取签到奖励列表
	void sendGetSignList();

	//发送签到领取
	void sendSignRecvice(int num);

	//解析拉取的奖励列表
	void parseSignList(const char* szData);

	//解析签到领取反馈
	void parseSignRecvice(const char* szData);

private:

	LayerSignController();

	static LayerSignController* mInstance;
};

#endif
