#ifndef SceneTakeCard_h
#define SceneTakeCard_h

#include "cocos2d.h"

class LayerTakeCard;

class SceneTakeCard:public cocos2d::Scene
{

public:
	SceneTakeCard();
	~SceneTakeCard();

	static SceneTakeCard* create(bool pushScene = false);
protected:

	bool init(bool pushScene);
	virtual void onExit();
	virtual void onEnter();

private:
	LayerTakeCard					*m_pLayerTakeCard;
};



#endif
