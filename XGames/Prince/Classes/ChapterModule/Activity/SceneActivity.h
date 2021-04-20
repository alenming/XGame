#ifndef SceneActivity_h__
#define SceneActivity_h__

#include "cocos2d.h"
#include "LayerActivity.h"

USING_NS_CC;

class SceneActivity : public Scene
{

public:

	SceneActivity();
	virtual ~SceneActivity();

	static SceneActivity* create(bool pushScene = false);

public:

	//≥ı ºªØ
	virtual bool init(bool pushScene);

	virtual void onEnter();

	virtual void onExit();

	static bool getPushScene() { return isPushScene; };

private:

	static bool isPushScene;
};



#endif // SceneActivity_h__