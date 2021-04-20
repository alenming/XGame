/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampModel
 *  Description:	星盘数据模型模块的UI实现;
 *  Version:		1.0
 *  Author:			wangxiangming
 *  Date:			02/19/2016
 *  Others:			
 *
 ******************************************************************/

#ifndef StarsCampStoneScene_h_
#define StarsCampStoneScene_h_

#include "StarsCampDef.h"
#include "StarsCampStoneLayer.h"


class StarsCampStoneScene:public Scene
{
public:
	~StarsCampStoneScene();

	CREATE_FUNC(StarsCampStoneScene);
	 
protected:
	virtual bool init() override;
	virtual void onEnter() override;
	virtual void onExit() override;

	StarsCampStoneScene();
private:

	StarsCampStoneLayer*  m_StarsCampStoneLayer;
};
#endif // !StarsCampScene_h_
