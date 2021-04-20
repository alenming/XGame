#include "GroupBossScene.h"
#include "GroupBossLayer.h"
#include "Utils/SoundUtils.h"

GroupBossScene::GroupBossScene()
{

}

GroupBossScene::~GroupBossScene()
{

}

GroupBossScene* GroupBossScene::create( bool isPush )
{
	GroupBossScene* scene = new GroupBossScene();
	if (scene->init(isPush))
	{
		scene->autorelease();
		return scene;
	}
	else
	{
		delete scene;
		return nullptr;
	}
}

bool GroupBossScene::init( bool isPush )
{
	if(!Scene::init())
	{
		return false;
	}

	GroupBossLayer* groupBossLayer = GroupBossLayer::create(isPush);
	addChild(groupBossLayer);

	return true;
}

void GroupBossScene::onExit()
{
	Scene::onExit();
}

void GroupBossScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

