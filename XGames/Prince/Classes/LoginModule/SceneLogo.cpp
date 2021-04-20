#include "SceneLogo.h"
#include "LayerLogo.h"

SceneLogo::SceneLogo()
{

}

SceneLogo::~SceneLogo()
{

}

bool SceneLogo::init()
{
	if (!Scene::init())
	{
		return false;
	}

	auto layerLogo = LayerLogo::create();
	this->addChild(layerLogo);
	return true;
}

