/************************************************

世界BOSS模块

************************************************/

#ifndef BossScene_h__
#define BossScene_h__

#include "cocos2d.h"
#include "BossLayer.h"

class BossScene : public cocos2d::Scene
{

public:
	BossScene();
	~BossScene();

	CREATE_FUNC(BossScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	BossLayer *m_pBossLayer;
};



#endif