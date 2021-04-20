#include "SceneMaze.h"
#include "LayerMaze.h"
#include "GuideCenter/GuideManager.h"

using namespace cocos2d;

bool SceneMaze::m_isPushScene = false;

SceneMaze::SceneMaze()
{

}

SceneMaze::~SceneMaze()
{

}

SceneMaze* SceneMaze::create(bool pushScene /*= false*/)
{
	SceneMaze *pRet = new SceneMaze();
	if (pRet && pRet->init(pushScene))
	{
		m_isPushScene = pushScene;
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

bool SceneMaze::init(bool pushScene)
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLayerMaze = LayerMaze::create(pushScene);
	this->addChild(m_pLayerMaze);

	return true;
}

void SceneMaze::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("chapter_3");
}

void SceneMaze::onExit()
{
	Scene::onExit();
}

