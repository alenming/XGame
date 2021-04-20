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

	//�����첽����
	auto asyncLoading = [this]()->void{ mIsAsyncLoaded = true; };
	ResourceLoader::getInstance()->addAsyncUIRes("UImainui2");
	ResourceLoader::getInstance()->addAsyncUIRes("UInewmain");
	ResourceLoader::getInstance()->addAsyncUIRes("Ani_zhuchengjiemian_DI");
	ResourceLoader::getInstance()->addAsyncImageRes("Image/UInewmain/floor.png");
	ResourceLoader::getInstance()->addAsyncImageRes("Image/UInewmain/JingTouGuang.png");
	ResourceLoader::getInstance()->startAsyncLoading(asyncLoading);

	if (!HeroModel::getInstance()->isInit())
	{
		//Ӣ��ģ��δ��ʼ�����ݣ�����Ӣ��������ȡЭ��
		LoginController::getInstance()->sendGetHeroDataMsg();

		//loading���治��ʾ�ջ�
		LayerWaiting::hideWaiting();
	}

	if (!ToolModel::getInstance()->isInit())
	{
		//����ģ��δ��ʼ�����ݣ����͵�����ȡЭ��
		LoginController::getInstance()->sendGetToolsMsg();

		//loading���治��ʾ�ջ�
		LayerWaiting::hideWaiting();
	}

	if (!ChapterModel::getInstance()->isInit())
	{
		// �½�����δ��ʼ����������ȡս����¼Э��;
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
	//Ӣ�����ݳ�ʼ���������������Դ�첽������ϣ���ʾ������
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

