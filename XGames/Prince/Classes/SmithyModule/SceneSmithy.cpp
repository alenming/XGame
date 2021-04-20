#include "SceneSmithy.h"
#include "LayerSmithy.h"
#include "GuideCenter/GuideManager.h"

using namespace cocos2d;

SceneSmithy::SceneSmithy()
{

}

SceneSmithy::~SceneSmithy()
{

}

SceneSmithy* SceneSmithy::create( const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, const SMITHY_OP_TYPE opType /*= SMITHY_OP_TYPE_NONE*/ )
{
	SceneSmithy* scene = new SceneSmithy();
	if (scene && scene->init(nHeroId, nToolId, equipType, opType))
	{
		scene->autorelease();
		return scene;
	}
	else
	{
		delete scene;
		return nullptr;
	}
}

bool SceneSmithy::init(const int nHeroId, const int nToolId, const EQUIP_ITEM_TYPE equipType, SMITHY_OP_TYPE opType)
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLayerSmithy = LayerSmithy::create();
	m_pLayerSmithy->initSmithyData(nHeroId, nToolId, equipType, opType);
	this->addChild(m_pLayerSmithy);

	return true;
}

void SceneSmithy::onEnter()
{
	Scene::onEnter();
}

void SceneSmithy::onExit()
{
	Scene::onExit();
}
