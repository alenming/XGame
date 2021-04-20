#ifndef MainCityLoadingScene_h__
#define MainCityLoadingScene_h__

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;
USING_NS_CC;

class MainCityLoadingScene : public Scene
{

public:

	MainCityLoadingScene() 
		: mIsAsyncLoaded(false)
	{};
	virtual ~MainCityLoadingScene() {};

	CREATE_FUNC(MainCityLoadingScene);

public:

	virtual bool init();

	virtual void onEnter();

	void update(float dt);

private:

	//异步加载是否完成
	bool mIsAsyncLoaded;
};



#endif //MainCityLoadingScene_h__