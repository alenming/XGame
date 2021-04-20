#include "SceneTool.h"
#include "LayerTool.h"
//#include "LayerRoleSelect.h"

using namespace cocos2d;

SceneTool::SceneTool()
{

}

SceneTool::~SceneTool()
{

}

bool SceneTool::init()
{
	if(!Scene::init())
	{
		return false;
	}
	Size size = Director::getInstance()->getWinSize();	

	m_pLayerTool = LayerTool::create();
	this->addChild(m_pLayerTool);
	
	return true;
}

void SceneTool::onEnter()
{
	Scene::onEnter();
	SoundUtils::playSoundEffect("quanping");

	// 音乐;
	SoundUtils::playSoundMusic("main_1");
}

void SceneTool::onExit()
{
	Scene::onExit();
}

