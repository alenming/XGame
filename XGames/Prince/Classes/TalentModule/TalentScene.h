/************************************************

Ö÷½ÇÌì¸³Ä£¿é

************************************************/

#ifndef TalentScene_h__
#define TalentScene_h__

#include "cocos2d.h"
#include "TalentLayer.h"

class TalentScene : public cocos2d::Scene
{

public:
	TalentScene();
	~TalentScene();

	CREATE_FUNC(TalentScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	TalentLayer *m_pTalentLayer;
};



#endif