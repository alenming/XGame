#include "ShopChouScene.h"


ShopChouScene::ShopChouScene()
{
	m_ShopChouLayer = nullptr;
}

ShopChouScene::~ShopChouScene()
{
}

Scene* ShopChouScene::createScene(CHOU_TYPE type )
{
	ShopChouScene *pRet = new ShopChouScene(); 
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

bool ShopChouScene::init(CHOU_TYPE type)
{
	if(!Scene::init())
	{
		return false;
	}
	m_type = type;
	m_ShopChouLayer = ShopChouLayer::create(type);
	this->addChild(m_ShopChouLayer);

	return true;
}

void ShopChouScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");
}

void ShopChouScene::onExit()
{
	Scene::onExit();
}
