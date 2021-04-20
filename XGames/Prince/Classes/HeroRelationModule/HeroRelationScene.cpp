#include "HeroRelationScene.h"
#include "HeroRelationLayer.h"
#include "Utils/SoundUtils.h"
#include "HeroModule/LayerHero.h"

HeroRelationScene::HeroRelationScene()
{

}

HeroRelationScene::~HeroRelationScene()
{

}

HeroRelationScene* HeroRelationScene::create(int heroId, bool isPush, LayerHero* layer )
{
	HeroRelationScene* scene = new HeroRelationScene();
	if (scene->init(heroId, isPush, layer))
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

bool HeroRelationScene::init(int heroId, bool isPush, LayerHero* layer )
{
	if(!Scene::init())
	{
		return false;
	}

	HeroRelationLayer* _heroRelationLayer = HeroRelationLayer::create(heroId, isPush, layer);
	addChild(_heroRelationLayer);

	return true;
}

void HeroRelationScene::onExit()
{
	Scene::onExit();
}

void HeroRelationScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

