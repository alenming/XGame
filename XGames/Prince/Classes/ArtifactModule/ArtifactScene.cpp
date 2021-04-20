#include "ArtifactScene.h"
#include "ArtifactLayer.h"
#include "Utils/SoundUtils.h"
#include "GuideCenter/GuideManager.h"

ArtifactScene::ArtifactScene()
{

}

ArtifactScene::~ArtifactScene()
{

}

bool ArtifactScene::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pArtifactLayer = ArtifactLayer::create();
	this->addChild(m_pArtifactLayer);

	return true;
}

void ArtifactScene::onExit()
{
	Scene::onExit();
}

void ArtifactScene::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundMusic("main_1");

	GuideManager::getInstance()->startGuide(GUIDE_INTERFACE_ARTIFACT);
}

