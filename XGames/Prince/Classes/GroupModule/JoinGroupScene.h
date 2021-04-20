/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		JoinGroupScene
 *  Description:	帮派加入前模块的Scene实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			01/18/2016
 *  Others:			
 *
 ******************************************************************/

#ifndef JoinGroupScene_h__
#define JoinGroupScene_h__

#include "JoinGroupDef.h"
#include "JoinGroupLayer.h"

class JoinGroupScene:public cocos2d::Scene
{

public:
	JoinGroupScene();
	~JoinGroupScene();

	CREATE_FUNC(JoinGroupScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	JoinGroupLayer* m_pJoinGroupLayer;
};



#endif
