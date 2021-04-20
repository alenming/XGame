#include "PvpScene.h"

bool PvpScene::m_isPushScene = false;

PvpScene::PvpScene()
{
	m_PvpLayer = nullptr;
}

PvpScene::~PvpScene()
{
}
PvpScene* PvpScene::create(bool pushScene /*= false*/)
{
	PvpScene *pRet = new PvpScene();
	if (pRet && pRet->init(pushScene))
	{
		m_isPushScene = pushScene;
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

bool PvpScene::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	m_PvpLayer = PvpLayer::create(pushScene);
	this->addChild(m_PvpLayer);

	return true;
}

void PvpScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_2");
}

void PvpScene::onExit()
{
	Scene::onExit();
}
