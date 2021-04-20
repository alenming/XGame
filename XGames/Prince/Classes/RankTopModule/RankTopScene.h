/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		RankTopScene
 *  Description:	排行榜模块;
 *  Version:		1.0
 *  Author:			Jerry
 *  Date:			13/11/2015
 *  Others: 
 *
 ******************************************************************/

#ifndef RankTopScene_h__
#define RankTopScene_h__

#include "cocos2d.h"
#include "RankTopLayer.h"

class RankTopScene:public cocos2d::Scene
{

public:
	RankTopScene();
	~RankTopScene();

	//CREATE_FUNC(RankTopScene);
	//pushScene传true, replaceScene传false
	static RankTopScene* createScene(bool isPush);

protected:
	virtual bool init(bool isPush);
	virtual void onExit();
	virtual void onEnter();

private:
	RankTopLayer *m_pRankTopLayer;
};



#endif
