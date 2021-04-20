/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		PvpScene
 *  Description:	Pvp模块的UI实现;
 *  Version:		1.0
 *  Author:			sonic
 *  Date:			07/30/2015
 *  Others:			
 *
 ******************************************************************/
#include "PvpHeader.h"
#include "PvpLayer.h"

#ifndef PvpScene_h_
#define  PvpScene_h_


class PvpScene:public Scene
{
public:
	~PvpScene();

	static PvpScene* create(bool pushScene = false);
		 
	PvpLayer* getPvplayer() {return m_PvpLayer;};

	static bool getPushScene() { return m_isPushScene;};

protected:
	bool init(bool pushScene);
	virtual void onEnter() override;
	virtual void onExit() override;


	PvpScene();
private:

	PvpLayer*  m_PvpLayer;

	static bool m_isPushScene;
};


#endif // !PvpScene_h_
