#include "NewAreaActivityScene.h"
#include "Utils/SoundUtils.h"

NewAreaActivityScene::NewAreaActivityScene()
{
	m_pNewAreaActLayer = nullptr;
}

NewAreaActivityScene::~NewAreaActivityScene()
{

}

bool NewAreaActivityScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pNewAreaActLayer = NewAreaActivityLayer::create();
	this->addChild(m_pNewAreaActLayer);

	return true;
}

void NewAreaActivityScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

void NewAreaActivityScene::onExit()
{
	Scene::onExit();
}
