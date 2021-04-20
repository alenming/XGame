#include "StarsSuScene.h"

StarsSUScene::StarsSUScene()
{
	m_StarsSuLayer = nullptr;
}

StarsSUScene::~StarsSUScene()
{
}

StarsSUScene* StarsSUScene::create(bool pushScene /*= false*/)
{
	StarsSUScene *pRet = new StarsSUScene();
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

bool StarsSUScene::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	m_StarsSuLayer = StarsSuLayer::create(pushScene);
	this->addChild(m_StarsSuLayer);

	return true;
}

void StarsSUScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_3");
}

void StarsSUScene::onExit()
{
	Scene::onExit();
}
