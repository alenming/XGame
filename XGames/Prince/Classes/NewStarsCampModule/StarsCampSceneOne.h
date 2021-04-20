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

#ifndef StarsCampNewOneCampScene_h_
#define StarsCampNewOneCampScene_h_

#include "StarsCampLayerOne.h"


class StarsCampSceneOne:public Scene
{
public:
	~StarsCampSceneOne();

	CREATE_FUNC(StarsCampSceneOne);
	 
protected:
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	StarsCampSceneOne();
private:

	StarsCampLayerOne*  m_StarsCampLayerOne;
};
#endif // !StarsCampScene_h_
