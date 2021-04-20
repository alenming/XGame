#include "SceneActivity.h"
#include "GuideCenter/GuideManager.h"

bool SceneActivity::isPushScene = false;

SceneActivity::SceneActivity()
{

}

SceneActivity::~SceneActivity()
{

}

SceneActivity* SceneActivity::create(bool pushScene /*= false*/)
{
	SceneActivity *pRet = new SceneActivity();
	if (pRet && pRet->init(pushScene))
	{
		isPushScene = pushScene;
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

bool SceneActivity::init(bool pushScene)
{
	if (!Scene::init())
	{
		return false;
	}

	auto layer = LayerActivity::create(pushScene);
	this->addChild(layer);

	return true;
}

void SceneActivity::onEnter()
{
	Scene::onEnter();

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ACTIVITY);
}

void SceneActivity::onExit()
{
	Scene::onExit();
}
