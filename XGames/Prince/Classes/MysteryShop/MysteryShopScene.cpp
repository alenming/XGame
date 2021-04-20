#include "MysteryShopScene.h"


MysteryShopScene::MysteryShopScene()
{
	m_ShopLayer = nullptr;
}

MysteryShopScene::~MysteryShopScene()
{
}

Scene* MysteryShopScene::createScene(SHOP_TYPE type)
{
	MysteryShopScene *pRet = new MysteryShopScene(); 
	if (pRet && pRet->init(type)) 
	{ 
		pRet->autorelease(); 
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}
bool MysteryShopScene::init(SHOP_TYPE type)
{
	if(!Scene::init())
	{
		return false;
	}

	m_ShopLayer = MysteryShopLayer::create(type);
	this->addChild(m_ShopLayer);

	return true;
}

void MysteryShopScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");
}

void MysteryShopScene::onExit()
{
	Scene::onExit();
}
