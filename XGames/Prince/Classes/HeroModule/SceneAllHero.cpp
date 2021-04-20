#include "SceneAllHero.h"
#include "GuideCenter/GuideManager.h"


SceneAllHero::SceneAllHero()
{

}

SceneAllHero::~SceneAllHero()
{

}

bool SceneAllHero::init()
{
	if (!Scene::init())
	{
		return false;
	}

	Layer* layer = LayerAllHero::create();
	addChild(layer);

	return true;
}

void SceneAllHero::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");
}

void SceneAllHero::onExit()
{
	Scene::onExit();

	ArmatureDataManager::destroyInstance();
	GUIReader::destroyInstance();
}

