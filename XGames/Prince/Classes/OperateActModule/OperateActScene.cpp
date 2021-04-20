#include "OperateActScene.h"

OperateActScene::OperateActScene()
{
	m_OperateActLayer = nullptr;
}

OperateActScene::~OperateActScene()
{
}

bool OperateActScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_OperateActLayer = OperateActLayer::create();
	this->addChild(m_OperateActLayer);

	return true;
}

void OperateActScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_2");
}

void OperateActScene::onExit()
{
	Scene::onExit();
}
