/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsCampScene
 *  Description:	情缘模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/23/2015
 *  Others:			
 *
 ******************************************************************/
#include "LoveHeader.h"
#include "LoveLayer.h"
#include "LoveController.h"

#ifndef LoveScene_h_
#define LoveScene_h_


class LoveScene:public Scene
{
public:
	~LoveScene();

	static LoveScene* create(bool pushScene = false);

protected:
	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;


	LoveScene();
private:

	LoveLayer*  m_LoveLayer;
	LoveController mLoveController;
};


#endif // !LoveScene_h_
