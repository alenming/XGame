/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		StarsSuScene
 *  Description:	星宿模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/10/2015
 *  Others:			
 *
 ******************************************************************/
#include "StarsSuHeader.h"
#include "StarsSuLayer.h"

#ifndef StarsSUScene_h_
#define  StarsSUScene_h_


class StarsSUScene:public Scene
{
public:
	~StarsSUScene();

	static StarsSUScene* create(bool pushScene = false);

protected:
	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;


	StarsSUScene();
private:

	StarsSuLayer*  m_StarsSuLayer;
};


#endif // !StarsSUScene_h_
