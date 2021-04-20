#ifndef MainCityScene_h__
#define MainCityScene_h__

#include "cocos2d.h"
#include "MainBuildingLayer.h"
#include "MainInfoLayer.h"
#include "spine/spine-cocos2dx.h"
#include "CocoStudio.h"
#include "CCArmature.h"
#include "HeroModule/SceneAllHero.h"

USING_NS_CC;
using namespace spine;
using namespace cocostudio;

class MainCityScene: public cocos2d::Scene
{
public:

	MainCityScene();
	~MainCityScene();

	CREATE_FUNC(MainCityScene);

public:

	virtual bool init() override;

	virtual void onEnter() override;

	virtual void onExit() override;

	MainBuildingLayer* getBuildingLayer() { return mPerspectLayer; }
	MainInfoLayer* getInfoLayer() { return mMainInfoLayer; }

private:

	//½¨ÖþÍ¸ÊÓ²ã
	MainBuildingLayer* mPerspectLayer;

	//UI²ã
	MainInfoLayer* mMainInfoLayer;

};

#endif // SceneHome_h__
