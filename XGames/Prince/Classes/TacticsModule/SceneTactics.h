/************************************************

阵法模块

************************************************/

#ifndef SceneTactics_h__
#define SceneTactics_h__

#include "cocos2d.h"
#include "LayerTactics.h"

class SceneTactics : public cocos2d::Scene
{

public:
	SceneTactics();
	~SceneTactics();

	//CREATE_FUNC(SceneTactics);
	static SceneTactics* create(bool isPush);

protected:
	virtual bool init(bool isPush);
	virtual void onExit();
	virtual void onEnter();

private:
	LayerTactics *m_pTacticsLayer;
};



#endif