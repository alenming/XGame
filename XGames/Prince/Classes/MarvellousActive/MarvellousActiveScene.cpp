#include "MarvellousActiveScene.h"
#include "GuideCenter/GuideManager.h"


MarvellousActiveScene::MarvellousActiveScene()
{
	m_MarvellousActiveLayer = nullptr;
}

MarvellousActiveScene::~MarvellousActiveScene()
{
}

bool MarvellousActiveScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_MarvellousActiveLayer = MarvellousActiveLayer::create();
	this->addChild(m_MarvellousActiveLayer);

	return true;
}

void MarvellousActiveScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_MARVELLOUSACTIVE);
}

void MarvellousActiveScene::onExit()
{
	Scene::onExit();
}
