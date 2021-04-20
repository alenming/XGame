#include "TalentScene.h"
#include "Utils/SoundUtils.h"

TalentScene::TalentScene()
{
	m_pTalentLayer = nullptr;
}

TalentScene::~TalentScene()
{

}

bool TalentScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pTalentLayer = TalentLayer::create();
	this->addChild(m_pTalentLayer);

	return true;
}

void TalentScene::onEnter()
{
	Scene::onEnter();

	SoundUtils::playSoundMusic("main_1");
}

void TalentScene::onExit()
{
	Scene::onExit();
}

