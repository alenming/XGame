#ifndef  AllPvpScene_h__
#define  AllPvpScene_h__

#include "cocos2d.h"
#include "CocosGUI.h"
#include "AllPvpLayer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class AllPvpScene:public cocos2d::Scene
{

public:
	AllPvpScene();
	~AllPvpScene();

	static AllPvpScene* create();

protected:
	bool init();
	virtual void onExit();
	virtual void onEnter();
private:
	AllPvpLayer *m_allPvpLayer;
};
#endif
