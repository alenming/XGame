#include "SceneFriend.h"
#include "LayerFriend.h"

using namespace cocos2d;

SceneFriend::SceneFriend()
{

}

SceneFriend::~SceneFriend()
{

}

bool SceneFriend::init()
{
	if(!Scene::init())
	{
		return false;
	}

	m_pLayerFriend = LayerFriend::create();
	this->addChild(m_pLayerFriend);

	return true;
}

void SceneFriend::onEnter()
{
	Scene::onEnter();
}

void SceneFriend::onExit()
{
	Scene::onExit();
}

