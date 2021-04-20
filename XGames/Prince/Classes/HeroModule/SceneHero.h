#ifndef SceneHero_h__
#define SceneHero_h__

#include "cocos2d.h"
#include "LayerHero.h"

USING_NS_CC;

class SceneHero : public Scene
{

public:

	SceneHero();
	virtual ~SceneHero();

	static SceneHero* create(int heroId = 0);

public:

	//≥ı ºªØ
	virtual bool init(int heroId);

	virtual void onEnter();

	virtual void onExit();

private:

	LayerHero* mLayerCharacter;

};



#endif // SceneHero_h__