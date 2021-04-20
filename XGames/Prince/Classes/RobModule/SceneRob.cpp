#include "SceneRob.h"
#include "LayerRob.h"
#include "GuideCenter/GuideManager.h"

SceneRob::SceneRob()
{

}

SceneRob::~SceneRob()
{

}

SceneRob* SceneRob::create(bool pushScene /*= false*/)
{
	SceneRob *pRet = new SceneRob();
	if (pRet && pRet->init(pushScene))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool SceneRob::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	LayerRob* _layer = LayerRob::create(pushScene);
	this->addChild(_layer);

	return true;
}

void SceneRob::onEnter()
{
	Scene::onEnter();
	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ROB);

	SoundUtils::playSoundEffect("quanping");
}

void SceneRob::onExit()
{
	Scene::onExit();
}

