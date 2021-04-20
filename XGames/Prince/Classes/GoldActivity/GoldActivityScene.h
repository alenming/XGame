#ifndef  GoldActivityScene_h__
#define  GoldActivityScene_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "Utils/ResourceUtils.h"
#include "GoldActivityLayer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class GoldActivityScene:public cocos2d::Scene
{

public:
	GoldActivityScene();
	~GoldActivityScene();

	static GoldActivityScene* create();

protected:
	bool init();
	virtual void onExit();
	virtual void onEnter();
private:
	GoldActivityLayer *m_goldActivity;

};
#endif
