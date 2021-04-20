#ifndef SceneFriend_h
#define SceneFriend_h

#include "cocos2d.h"

class LayerFriend;

class SceneFriend:public cocos2d::Scene
{

public:
	SceneFriend();
	~SceneFriend();

	CREATE_FUNC(SceneFriend);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	LayerFriend					*m_pLayerFriend;
};



#endif
