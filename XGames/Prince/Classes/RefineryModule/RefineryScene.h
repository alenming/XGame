/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		RefineryScene
 *  Description:	炼化炉模块的scene实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			10/22/2015
 *  Others:			
 *
 ******************************************************************/
#include "RefineryHeader.h"
#include "RefineryLayer.h"

#ifndef RefineryScene_h_
#define RefineryScene_h_


class RefineryScene:public Scene
{
public:
	~RefineryScene();

	static RefineryScene* create(bool pushScene = false);
	 
protected:
	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;


	RefineryScene();
private:

	RefineryLayer*  m_RefineryLayer;
};


#endif // RefineryScene_h_
