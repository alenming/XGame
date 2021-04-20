#include "SceneLogout.h"
#include "Widget/LayerLoading.h"
#include "LoginController.h"
#include "SceneLogin.h"


SceneLogout::SceneLogout()
{

}

SceneLogout::~SceneLogout()
{

}

bool SceneLogout::init()
{
	if (!Scene::init())
	{
		return false;
	}

	LayerLoading* loading = LayerLoading::create();
	this->addChild(loading);

	return true;
}

void SceneLogout::onExit()
{
	Scene::onExit();
}

void SceneLogout::onEnter()
{
	Scene::onEnter();

	//下一帧执行注销操作
	auto callBack = [this](float dt)->void
	{
		LoginController::getInstance()->logoutAccount();

		//返回登录主场景
		Director::getInstance()->popToRootScene();
		Director::getInstance()->replaceScene(SceneLogin::create());
	};

	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "SceneLogout::onEnter");
}

