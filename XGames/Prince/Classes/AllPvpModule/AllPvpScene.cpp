#include "AllPvpScene.h"
#include "GuideCenter/GuideManager.h"

AllPvpScene::AllPvpScene(void)
{
}


AllPvpScene::~AllPvpScene(void)
{
}


void AllPvpScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");
	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ARENA_SUB_PAGE);
}

void AllPvpScene::onExit()
{
	Scene::onExit();
}


bool AllPvpScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_allPvpLayer = AllPvpLayer::create();
	this->addChild(m_allPvpLayer);

	return true;
}

AllPvpScene* AllPvpScene::create()
{
	AllPvpScene *pRet = new AllPvpScene();
	if (pRet && pRet->init())
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
