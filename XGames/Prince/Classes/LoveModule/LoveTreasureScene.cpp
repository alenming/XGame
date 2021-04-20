#include "LoveTreasureScene.h"

LoveTreasureScene::LoveTreasureScene(void)
{
}


LoveTreasureScene::~LoveTreasureScene(void)
{
}

LoveTreasureScene* LoveTreasureScene::create(int idx,int petId)
{
	//背景设置
	LoveTreasureScene* ret = new LoveTreasureScene();
	if (ret && ret->init(idx,petId))
	{
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool LoveTreasureScene::init(int idx,int petId)
{
	if(!Scene::init())
	{
		return false;
	}

	m_LoveTreasureLayer = LoveTreasureLayer::create(idx,petId);
	this->addChild(m_LoveTreasureLayer);

	return true;
}

void LoveTreasureScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");
}

void LoveTreasureScene::onExit()
{
	Scene::onExit();
}
