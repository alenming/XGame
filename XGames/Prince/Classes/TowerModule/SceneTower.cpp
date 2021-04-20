#include "SceneTower.h"
#include "GuideCenter/GuideManager.h"

bool SceneTower::mIsPushScene = false;

SceneTower::SceneTower()
	: mLayerTowerMain(nullptr)
{

}

SceneTower::~SceneTower()
{

}

SceneTower* SceneTower::create(TOWER_TYPE type, bool pushScene)
{
	SceneTower* _scene = new SceneTower;
	if (nullptr != _scene && _scene->init(type))
	{
		mIsPushScene = pushScene;
		_scene->autorelease();
		return _scene;
	}

	CC_SAFE_DELETE(_scene);
	return nullptr;
}

void SceneTower::onEnter()
{
	Scene::onEnter();

	// 音乐;
	SoundUtils::playSoundMusic("main_2");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_PRACTICE);
}

bool SceneTower::init(TOWER_TYPE type)
{
	if (!Scene::init())
	{
		return false;
	}

	initUI(type);

	return true;
}


void SceneTower::initUI(TOWER_TYPE type /* = TOWER_TYPE_INVALID */)
{
	mLayerTowerMain = LayerTowerMain::create(type, mIsPushScene);
	if (mLayerTowerMain)
	{
		this->addChild(mLayerTowerMain);
	}
}