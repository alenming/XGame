#include "RefineryScene.h"
#include "GuideCenter/GuideManager.h"

RefineryScene::RefineryScene()
{
	m_RefineryLayer = nullptr;
}

RefineryScene::~RefineryScene()
{
}

RefineryScene* RefineryScene::create(bool pushScene /*= false*/)
{
	RefineryScene *pRet = new RefineryScene();
	if (pRet && pRet->init(pushScene))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

bool RefineryScene::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	m_RefineryLayer = RefineryLayer::create(pushScene);
	this->addChild(m_RefineryLayer);

	return true;
}

void RefineryScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_2");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_REFINE);
}

void RefineryScene::onExit()
{
	Scene::onExit();
}
