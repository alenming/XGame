#ifndef MyGroupScene_h__
#define MyGroupScene_h__

#include "cocos2d.h"
#include "MyGroupLayer.h"


class MyGroupScene:public cocos2d::Scene
{

public:
	MyGroupScene();
	~MyGroupScene();

	CREATE_FUNC(MyGroupScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	MyGroupLayer* m_pMyGroupLayer;
};



#endif
