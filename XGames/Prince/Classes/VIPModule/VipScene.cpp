#include "VipScene.h"

VipScene::VipScene()
{
	m_pVipLayer = nullptr;
}

VipScene::~VipScene()
{

}

VipScene* VipScene::createScene(Panel_Type type, int vipLevel)
{
	VipScene* scene = new VipScene();
	if (scene->init(type, vipLevel))
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

bool VipScene::init(Panel_Type type, int vipLevel)
{
	if(!Scene::init())
	{
		return false;
	}

	m_pVipLayer = VipLayer::create(type, vipLevel);
	this->addChild(m_pVipLayer);

	return true;
}

void VipScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

void VipScene::onExit()
{
	Scene::onExit();
}