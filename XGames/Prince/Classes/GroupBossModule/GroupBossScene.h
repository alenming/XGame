/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		GroupBossScene
 *  Description:	帮派加入前模块的Scene实现;
 *  Version:		1.0
 *  Author:			jerry
 *  Date:			23/5/2016
 *  Others:			
 *
 ******************************************************************/

#ifndef GroupBossScene_h__
#define GroupBossScene_h__

#include "GroupBossLayer.h"

class GroupBossScene:public cocos2d::Scene
{

public:
	GroupBossScene();
	~GroupBossScene();

	static GroupBossScene* create(bool isPush);

protected:
	virtual bool init(bool isPush);
	virtual void onExit();
	virtual void onEnter();

private:
	
};



#endif
