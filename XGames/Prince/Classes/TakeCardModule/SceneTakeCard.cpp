#include "SceneTakeCard.h"
#include "LayerTakeCard.h"
#include "GuideCenter/GuideManager.h"
#include "Utils/SoundUtils.h"

using namespace cocos2d;

SceneTakeCard::SceneTakeCard()
{

}

SceneTakeCard::~SceneTakeCard()
{

}


SceneTakeCard* SceneTakeCard::create(bool pushScene /*= false*/)
{
	SceneTakeCard *pRet = new SceneTakeCard();
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

bool SceneTakeCard::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLayerTakeCard = LayerTakeCard::create(pushScene);
	this->addChild(m_pLayerTakeCard);

	return true;
}

void SceneTakeCard::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("store_1");
}

void SceneTakeCard::onExit()
{
	Scene::onExit();
}