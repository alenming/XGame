#include "SceneTactics.h"
#include "Utils/SoundUtils.h"
#include "GuideCenter/GuideManager.h"

SceneTactics::SceneTactics()
{
	m_pTacticsLayer = nullptr;
}

SceneTactics::~SceneTactics()
{

}

bool SceneTactics::init(bool isPush)
{
	if(!Scene::init())
	{
		return false;
	}

	m_pTacticsLayer = LayerTactics::create(isPush);
	this->addChild(m_pTacticsLayer);

	return true;
}

void SceneTactics::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_TACTICS);
}

void SceneTactics::onExit()
{
	Scene::onExit();
}

SceneTactics* SceneTactics::create( bool isPush )
{
	SceneTactics* scene = new SceneTactics();
	if (scene->init(isPush))
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

