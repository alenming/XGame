#ifndef SceneRob_h__
#define SceneRob_h__

#include "RobDef.h"

class SceneRob : public Scene
{
public:
	SceneRob();
	~SceneRob();

	static SceneRob* create(bool pushScene = false);

protected:
	virtual bool init(bool pushScene);
	virtual void onExit();
	virtual void onEnter();

};

#endif // SceneRob_h__
