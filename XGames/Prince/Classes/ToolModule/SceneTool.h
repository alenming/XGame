#ifndef SceneTool_h
#define SceneTool_h

#include "cocos2d.h"

class LayerTool;
class LayerRoleSelect;

class SceneTool:public cocos2d::Scene
{

public:
	SceneTool();
	~SceneTool();

	CREATE_FUNC(SceneTool);

protected:
	virtual bool init();
	virtual void onExit();
	virtual void onEnter();

private:
	LayerTool					*m_pLayerTool;
	LayerRoleSelect				*m_pLayerRole;
};



#endif
