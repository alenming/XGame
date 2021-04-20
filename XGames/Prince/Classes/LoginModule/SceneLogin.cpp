#include "SceneLogin.h"
#include "LayerLogin.h"
#include "LayerRoleSelect.h"
#include "Utils/ValueCrypt.h"
#include "Temp/CustomTips.h"
#include "Communication/EnvConfig.h"
#include "MainModule/MainBuildingLayer.h"
#include "DataManager/DataManager.h"

using namespace cocos2d;

SceneLogin::SceneLogin()
{

}

SceneLogin::~SceneLogin()
{

}

bool SceneLogin::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLayerLogin = LayerLogin::create();
	this->addChild(m_pLayerLogin);

	//每次进入登录界面，都需要重新初始化主场景滚动比例
	MainBuildingLayer::setInitialPercent();
	
	return true;
}

void SceneLogin::onEnter()
{
	Scene::onEnter();

#ifdef LOGIN_TEST
	//CustomTips::show("正在使用测试环境");
	CustomTips::showUtf8(DataManager::getInstance()->searchCommonTexdtById(10285));
#endif

}

void SceneLogin::onExit()
{
	Scene::onExit();
}

