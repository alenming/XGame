#include "RankTopScene.h"
#include "Utils/SoundUtils.h"

RankTopScene::RankTopScene()
{
	m_pRankTopLayer = nullptr;
}

RankTopScene::~RankTopScene()
{

}

RankTopScene* RankTopScene::createScene(bool isPush)
{
	RankTopScene* scene = new RankTopScene();
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

void RankTopScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

void RankTopScene::onExit()
{
	Scene::onExit();
}

bool RankTopScene::init(bool isPush)
{
	if(!Scene::init())
	{
		return false;
	}

	RankTopLayer* rankTopLayer = RankTopLayer::create(isPush);
	addChild(rankTopLayer);

	return true;
}
