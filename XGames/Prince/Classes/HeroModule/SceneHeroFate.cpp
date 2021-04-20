#include "SceneHeroFate.h"
#include "LayerHeroFate.h"


SceneHeroFate::SceneHeroFate()
{

}

SceneHeroFate::~SceneHeroFate()
{

}

bool SceneHeroFate::init()
{
	if (!Scene::init())
	{
		return false;
	}

	LayerHeroFate* layer = LayerHeroFate::create();
	this->addChild(layer);
	return true;
}

void SceneHeroFate::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");
}

void SceneHeroFate::onExit()
{
	Scene::onExit();
}

