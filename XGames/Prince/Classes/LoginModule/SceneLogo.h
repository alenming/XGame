#ifndef SceneLogo_h__
#define SceneLogo_h__

#include "cocos2d.h"
#include "LayerLogo.h"

USING_NS_CC;

class SceneLogo : public Scene
{

public:
	SceneLogo();
	virtual ~SceneLogo();

	CREATE_FUNC(SceneLogo);

public:

	virtual bool init();
};



#endif //SceneLogo_h__