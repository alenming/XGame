/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		OperateActScene
 *  Description:	OperateAct运营活动模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			12/7/2015
 *  Others:			
 *
 ******************************************************************/
#include "OperateActHeader.h"
#include "OperateActLayer.h"

#ifndef  OperateActScene_h_
#define  OperateActScene_h_


class OperateActScene:public Scene
{
public:
	~OperateActScene();

	CREATE_FUNC(OperateActScene);
	 
protected:
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;


	OperateActScene();
private:

	OperateActLayer*  m_OperateActLayer;
};


#endif // !OperateActScene_h_
