#include "MyGroupScene.h"
#include "Utils/SoundUtils.h"

MyGroupScene::MyGroupScene()
{
	m_pMyGroupLayer = nullptr;
}

MyGroupScene::~MyGroupScene()
{

}

bool MyGroupScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	//进入我的帮派主页
	m_pMyGroupLayer = MyGroupLayer::create();
	this->addChild(m_pMyGroupLayer);

	return true;
}

void MyGroupScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

void MyGroupScene::onExit()
{
	Scene::onExit();
}