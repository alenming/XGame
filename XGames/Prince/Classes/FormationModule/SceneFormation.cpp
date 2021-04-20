#include "SceneFormation.h"
#include "LayerFormation.h"
#include "MainModule/MainCityLoadingScene.h"

SceneFormation::SceneFormation(void)
{
}


SceneFormation::~SceneFormation(void)
{
}


SceneFormation* SceneFormation::create( Formation_Mode mode/* = F_MODE_FORMATION*/ )
{
	SceneFormation* _scene = new SceneFormation;
	if (nullptr != _scene && _scene->init(mode))
	{
		_scene->autorelease();
		return _scene;
	}

	CC_SAFE_DELETE(_scene);
	return nullptr;
}


bool SceneFormation::init( Formation_Mode mode )
{
	if (!Scene::init())
	{
		return false;
	}
	m_mode = mode;

	initUI(mode);
	return true;
}


void SceneFormation::initUI( Formation_Mode mode )
{
	LayerFormation* pLayer = LayerFormation::create(mode);
	this->addChild(pLayer);
}


void SceneFormation::update(float delta)
{
	this->unscheduleUpdate();
}


void SceneFormation::onBtnClose()
{
	if (m_mode == F_MODE_FORMATION)
	{
		Director::getInstance()->replaceScene(MainCityScene::create());
	}
	else
	{
		Director::getInstance()->popScene();
	}
}