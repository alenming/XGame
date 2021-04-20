#ifndef SceneSmithy_h
#define SceneSmithy_h

#include "cocos2d.h"
#include "ToolModule/ToolDef.h"
#include "SmithyModule/SmithyDef.h"

class LayerSmithy;

class SceneSmithy : public cocos2d::Scene
{

public:

	SceneSmithy();
	~SceneSmithy();

	static SceneSmithy* create(const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, const SMITHY_OP_TYPE opType = SMITHY_OP_TYPE_NONE);

protected:

	virtual bool init(const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, SMITHY_OP_TYPE opType);
	virtual void onExit();
	virtual void onEnter();

private:

	LayerSmithy	*m_pLayerSmithy;
};



#endif
