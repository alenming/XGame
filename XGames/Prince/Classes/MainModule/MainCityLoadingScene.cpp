#include "MainCityLoadingScene.h"
#include "Widget/LayerLoading.h"
#include "ResourceManager/ResourceLoader.h"
#include "LoginModule/LoginController.h"
#include "HeroModule/HeroModel.h"
#include "MainCityScene.h"
#include "ToolModule/ToolModel.h"
#include "Widget/LayerWaiting.h"
#include "MainTaskModule/MainTaskController.h"
#include "ChapterModule/ChapterController.h"
#include "ChapterModule/ChapterModel.h"

bool MainCityLoadingScene::init()
{
	if (!Scene::init())
	{
		return false;
	}

	LayerLoading* layerLoading = LayerLoading::create();
	addChild(layerLoading);

	this->scheduleUpdate();

	return true;
}

void MainCityLoadingScene::onEnter()
{
	Scene::onEnter();

	//启动异步加载
	auto asyncLoading = [this]()->void{ mIsAsyncLoaded = true; };
	ResourceLoader::getInstance()->addAsyncUIRes("UImainui2");
	ResourceLoader::getInstance()->addAsyncUIRes("UInewmain");
	ResourceLoader::getInstance()->addAsyncUIRes("Ani_zhuchengjiemian_DI");
	ResourceLoader::getInstance()->addAsyncImageRes("Image/UInewmain/floor.png");
	ResourceLoader::getInstance()->addAsyncImageRes("Image/UInewmain/JingTouGuang.png");
	ResourceLoader::getInstance()->startAsyncLoading(asyncLoading);

	if (!HeroModel::getInstance()->isInit())
	{
		//英雄模块未初始化数据，则发送英雄数据拉取协议
		LoginController::getInstance()->sendGetHeroDataMsg();

		//loading界面不显示菊花
		LayerWaiting::hideWaiting();
	}

	if (!ToolModel::getInstance()->isInit())
	{
		//道具模块未初始化数据，则发送道具拉取协议
		LoginController::getInstance()->sendGetToolsMsg();

		//loading界面不显示菊花
		LayerWaiting::hideWaiting();
	}

	if (!ChapterModel::getInstance()->isInit())
	{
		// 章节数据未初始化，则发送拉取战斗记录协议;
		ChapterController::getInstance()->handlePveData();
		LayerWaiting::hideWaiting();
	}

	MainTaskController::getInstance()->sendGetTaskListMsg();

	LayerWaiting::hideWaiting();

	Director::getInstance()->getTextureCache()->removeUnusedTextures();
	//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
}

void MainCityLoadingScene::update(float dt)
{
	//英雄数据初始化完毕且主场景资源异步加载完毕，显示主场景
	if (mIsAsyncLoaded && HeroModel::getInstance()->isInit()
		&& ToolModel::getInstance()->isInit()
		&& ChapterModel::getInstance()->isInit())
	{
		//SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound/menu.mp3", true);
		Director::getInstance()->replaceScene(MainCityScene::create());
		//log(Director::getInstance()->getTextureCache()->getCachedTextureInfo().c_str());
		this->unscheduleUpdate();
	}
}

