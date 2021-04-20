#include "BossScene.h"
#include "Utils/SoundUtils.h"

BossScene::BossScene()
{
	m_pBossLayer = nullptr;
}

BossScene::~BossScene()
{

}

bool BossScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pBossLayer = BossLayer::create();
	this->addChild(m_pBossLayer);

	return true;
}

void BossScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

void BossScene::onExit()
{
	Scene::onExit();
}

