#include "GoldActivityScene.h"

GoldActivityScene::GoldActivityScene(void)
{
}


GoldActivityScene::~GoldActivityScene(void)
{
}


void GoldActivityScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");
}

void GoldActivityScene::onExit()
{
	Scene::onExit();
}


bool GoldActivityScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_goldActivity = GoldActivityLayer::create();
	this->addChild(m_goldActivity);

	return true;
}

GoldActivityScene* GoldActivityScene::create()
{
	GoldActivityScene *pRet = new GoldActivityScene();
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
