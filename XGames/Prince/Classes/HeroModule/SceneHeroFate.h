#ifndef SceneHeroFate_h__
#define SceneHeroFate_h__

#include "cocos2d.h"

USING_NS_CC;

class SceneHeroFate : public Scene
{

public:

	SceneHeroFate();
	virtual ~SceneHeroFate();

	CREATE_FUNC(SceneHeroFate);

public:

	//≥ı ºªØ
	virtual bool init();

	virtual void onEnter();
	virtual void onExit();


};



#endif // SceneHeroFate_h__