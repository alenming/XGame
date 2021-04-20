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

	//��һִ֡��ע������
	auto callBack = [this](float dt)->void
	{
		LoginController::getInstance()->logoutAccount();

		//���ص�¼������
		Director::getInstance()->popToRootScene();
		Director::getInstance()->replaceScene(SceneLogin::create());
	};

	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "SceneLogout::onEnter");
}

