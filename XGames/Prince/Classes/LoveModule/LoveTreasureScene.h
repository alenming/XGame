#ifndef LoveTreasureScene_h__
#define LoveTreasureScene_h__

#include "LoveHeader.h"
#include "LoveTreasureLayer.h"

USING_NS_CC;
using namespace cocos2d::ui;
using namespace std;

class LoveTreasureScene:public Scene
{
public:
	~LoveTreasureScene(void);
	LoveTreasureScene(void);

	static LoveTreasureScene* create(int idx,int petId);

	virtual bool init(int idx,int petId);
	virtual void onEnter() override;
	virtual void onExit() override;
private:

	LoveTreasureLayer* m_LoveTreasureLayer;
};

#endif
