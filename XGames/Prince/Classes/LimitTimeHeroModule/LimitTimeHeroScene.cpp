#include "LimitTimeHeroScene.h"
#include "LimitTimeHeroLayer.h"
#include "Utils/SoundUtils.h"

LimitTimeHeroScene::LimitTimeHeroScene()
{

}

LimitTimeHeroScene::~LimitTimeHeroScene()
{

}

bool LimitTimeHeroScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLimitTimeHeroLayer = LimitTimeHeroLayer::create();
	this->addChild(m_pLimitTimeHeroLayer);

	return true;
}

void LimitTimeHeroScene::onExit()
{
	Scene::onExit();
}

void LimitTimeHeroScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundMusic("main_1");
}

