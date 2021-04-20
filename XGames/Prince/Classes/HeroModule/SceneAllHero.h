#ifndef SceneAllHero_h__
#define SceneAllHero_h__

#include "cocos2d.h"
#include "LayerAllHero.h"

USING_NS_CC;

class SceneAllHero : public Scene
{

public:

	SceneAllHero();
	virtual ~SceneAllHero();

	CREATE_FUNC(SceneAllHero);

public:

	virtual bool init();
	virtual void onEnter();
	virtual void onExit();

};


#endif //SceneAllHero_h__