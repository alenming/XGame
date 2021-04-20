/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampScene
 *  Description:	星盘模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#include "StarsCampLayer.h"

#ifndef StarsCampScene_h_
#define  StarsCampScene_h_

class StarsCampScene:public Scene
{
public:
	~StarsCampScene();

	CREATE_FUNC(StarsCampScene);
	 
protected:
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	StarsCampScene();
private:

	StarsCampLayer*  m_StarsCampLayer;
};
#endif // !StarsCampScene_h_
