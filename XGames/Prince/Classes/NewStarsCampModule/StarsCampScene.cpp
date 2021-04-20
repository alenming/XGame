#include "StarsCampScene.h"


StarsCampScene::StarsCampScene()
{
	m_StarsCampLayer = nullptr;
}

StarsCampScene::~StarsCampScene()
{
}

bool StarsCampScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_StarsCampLayer = StarsCampLayer::create();
	this->addChild(m_StarsCampLayer);

	return true;
}

void StarsCampScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");
}

void StarsCampScene::onExit()
{
	Scene::onExit();
}
