/************************************************

神器模块

************************************************/

#ifndef ArtifactScene_h__
#define ArtifactScene_h__

#include "cocos2d.h"
#include "ArtifactLayer.h"

class ArtifactScene : public cocos2d::Scene
{

public:
	ArtifactScene();
	~ArtifactScene();

	CREATE_FUNC(ArtifactScene);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	ArtifactLayer *m_pArtifactLayer;
};



#endif