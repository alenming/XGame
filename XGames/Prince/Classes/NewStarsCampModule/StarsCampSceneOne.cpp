#include "StarsCampSceneOne.h"


StarsCampSceneOne::StarsCampSceneOne()
{
	m_StarsCampLayerOne = nullptr;
}

StarsCampSceneOne::~StarsCampSceneOne()
{
}

bool StarsCampSceneOne::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_StarsCampLayerOne = StarsCampLayerOne::create();
	this->addChild(m_StarsCampLayerOne);

	return true;
}

void StarsCampSceneOne::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");
}

void StarsCampSceneOne::onExit()
{
	Scene::onExit();
}
