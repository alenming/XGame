/************************************************

新区活动模块

************************************************/

#ifndef NewAreaActivityScene_h__
#define NewAreaActivityScene_h__

#include "cocos2d.h"
#include "NewAreaActivityLayer.h"

class NewAreaActivityScene : public cocos2d::Scene
{

public:
	NewAreaActivityScene();
	~NewAreaActivityScene();

	CREATE_FUNC(NewAreaActivityScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	NewAreaActivityLayer *m_pNewAreaActLayer;
};



#endif