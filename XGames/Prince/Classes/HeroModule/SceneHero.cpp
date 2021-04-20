#include "SceneHero.h"
#include "GuideCenter/GuideManager.h"

SceneHero::SceneHero()
{

}

SceneHero::~SceneHero()
{

}

SceneHero* SceneHero::create(int heroId)
{
	SceneHero* scene = new SceneHero();
	if (scene && scene->init(heroId))
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

bool SceneHero::init(int heroId)
{
	if (!Scene::init())
	{
		return false;
	}

	mLayerCharacter = LayerHero::create();
	this->addChild(mLayerCharacter);
	mLayerCharacter->showHeroList(heroId);

	return true;
}

void SceneHero::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");
}

void SceneHero::onExit()
{
	Scene::onExit();

	ArmatureDataManager::getInstance()->destroyInstance();
	GUIReader::destroyInstance();
}

