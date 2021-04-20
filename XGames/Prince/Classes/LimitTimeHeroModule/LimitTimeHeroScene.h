/************************************************

限时神将模块

************************************************/

#ifndef LimitTimeHeroScene_h__
#define LimitTimeHeroScene_h__

#include "cocos2d.h"
#include "LimitTimeHeroLayer.h"

class LimitTimeHeroScene : public cocos2d::Scene
{

public:
	LimitTimeHeroScene();
	~LimitTimeHeroScene();

	CREATE_FUNC(LimitTimeHeroScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	LimitTimeHeroLayer *m_pLimitTimeHeroLayer;
};



#endif