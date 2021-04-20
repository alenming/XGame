#ifndef SceneLogout_h__
#define SceneLogout_h__

#include "cocos2d.h"

USING_NS_CC;

class SceneLogout : public Scene
{

public:

	SceneLogout();
	~SceneLogout();

	CREATE_FUNC(SceneLogout);

public:

	virtual bool init();
	virtual void onExit();
	virtual void onEnter();
};



#endif // SceneLogout_h__