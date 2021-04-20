/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampNewController
 *  Description:	星盘控制模型模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			02/19/2016
 *  Others:			
 *
 ******************************************************************/
#include "StarsCampDef.h"
#include "StarsCampModel.h"

#ifndef StarsCampController_h_
#define StarsCampController_h_

class StarsCampController:public ICommListener
{
public:
	StarsCampController();
	~StarsCampController();
	//活动单例
	static StarsCampController* getInstance();

	virtual void onRecvMessage(int nCmdID, const char* szData) override;

	void processCampInfoData(const char* szData); //解析星盘信息
	void processCampShopDate(const char *szData ); //购买
	void processCampXiangQianDate(const char *szData ); //镶嵌

	//获取星盘信息
	void sendGetStarsCampInfo();

	void sendShopStone(int id);

	void sendXiangQianStone(int id);

	bool isShowNewCampRedFlag();
private:
	static StarsCampController* mInstance;

	int coStoId;
};


#endif