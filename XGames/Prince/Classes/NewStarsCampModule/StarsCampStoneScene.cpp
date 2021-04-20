#include "StarsCampStoneScene.h"


StarsCampStoneScene::StarsCampStoneScene()
{
	m_StarsCampStoneLayer = nullptr;
}

StarsCampStoneScene::~StarsCampStoneScene()
{
}

bool StarsCampStoneScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_StarsCampStoneLayer = StarsCampStoneLayer::create();
	this->addChild(m_StarsCampStoneLayer);

	return true;
}

void StarsCampStoneScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_STAR_PLATE_BUY);
}

void StarsCampStoneScene::onExit()
{
	Scene::onExit();
}
