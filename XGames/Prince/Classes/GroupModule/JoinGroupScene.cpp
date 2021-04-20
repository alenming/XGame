#include "JoinGroupScene.h"

JoinGroupScene::JoinGroupScene()
{
	m_pJoinGroupLayer = nullptr;
}

JoinGroupScene::~JoinGroupScene()
{

}

bool JoinGroupScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	//进入我的帮派主页
	m_pJoinGroupLayer = JoinGroupLayer::create();
	this->addChild(m_pJoinGroupLayer);

	return true;
}

void JoinGroupScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundMusic("main_1");
}

void JoinGroupScene::onExit()
{
	Scene::onExit();
}
