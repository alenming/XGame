/******************************************************************* 
 *  Copyright(c) 2014 zcjoy  All rights reserved. 
 *
 *  FileName:		HeroRelationScene
 *  Description:	英雄缘分模块的Scene实现;
 *  Version:		1.0
 *  Author:			jerry
 *  Date:			6/6/2016
 *  Others:			
 *
 ******************************************************************/

#ifndef HeroRelationScene_h__
#define HeroRelationScene_h__

#include "HeroRelationLayer.h"

class HeroRelationScene:public cocos2d::Scene
{

public:
	HeroRelationScene();
	~HeroRelationScene();

	static HeroRelationScene* create(int heroId, bool isPush, LayerHero* layer);

protected:
	virtual bool init(int heroId, bool isPush, LayerHero* layer);
	virtual void onExit();
	virtual void onEnter();

private:
	
};



#endif
