#include "ShopScene.h"


ShopScene::ShopScene()
{
	m_ShopLayer = nullptr;
}

ShopScene::~ShopScene()
{
}

Scene* ShopScene::createScene(SHOP_TYPE type,bool isPush)
{
	ShopScene *pRet = new ShopScene(); 
	if (pRet && pRet->init(type,isPush)) 
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
bool ShopScene::init(SHOP_TYPE type,bool isPush)
{
	if(!Scene::init())
	{
		return false;
	}

	m_ShopLayer = ShopLayer::create(type, isPush);
	this->addChild(m_ShopLayer);

	return true;
}

void ShopScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");
}

void ShopScene::onExit()
{
	Scene::onExit();
}
