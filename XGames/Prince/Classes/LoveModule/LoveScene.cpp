#include "LoveScene.h"

LoveScene::LoveScene()
{
	m_LoveLayer = nullptr;
}

LoveScene::~LoveScene()
{
}

LoveScene* LoveScene::create(bool pushScene /*= false*/)
{
	LoveScene *pRet = new LoveScene();
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

bool LoveScene::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	m_LoveLayer = LoveLayer::create(pushScene);
	this->addChild(m_LoveLayer);

	return true;
}

void LoveScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_3");
}

void LoveScene::onExit()
{
	Scene::onExit();
}

