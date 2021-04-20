#ifndef Client_CScenePortal_h
#define Client_CScenePortal_h

#include "cocos2d.h"

class LayerLogin;
class LayerRoleSelect;

class SceneLogin:public cocos2d::Scene
{

public:
	SceneLogin();
	~SceneLogin();

	CREATE_FUNC(SceneLogin);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	LayerLogin					*m_pLayerLogin;
	LayerRoleSelect				*m_pLayerRole;
};



#endif
