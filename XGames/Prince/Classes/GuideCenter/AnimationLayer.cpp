#include "AnimationLayer.h"
#include "CCArmature.h"
#include "GuideManager.h"

using namespace cocostudio;

AnimationLayer::AnimationLayer()
{

}

AnimationLayer::~AnimationLayer()
{

}

AnimationLayer* AnimationLayer::create( const string& aniName, const string& actionName )
{
	AnimationLayer* chatLayer = new AnimationLayer();
	if (chatLayer->init(aniName, actionName))
	{
		chatLayer->autorelease();
		return chatLayer;
	}
	else
	{
		delete chatLayer;
		return nullptr;
	}
}

bool AnimationLayer::init( const string& aniName, const string& actionName )
{
	if (!ModalLayer::init())
	{
		return false;
	}

	//透明度设置为0
	this->setOpacity(0);

	//动画播放完毕自动销毁，同时执行下一条引导
	auto aniCallBack = [this](Armature *armature, MovementEventType movementType, const std::string& movementID)->void
	{
		if (movementType == MovementEventType::COMPLETE)
		{
			dispose();
			GuideManager::getInstance()->next();
		}
	};

	ArmatureDataManager::getInstance()->addArmatureFileInfo(
		"Image/AniStory/" + aniName + "/" + aniName + ".ExportJson");
	Armature* ani = Armature::create(aniName);
	ani->getAnimation()->play(actionName);
	ani->getAnimation()->setMovementEventCallFunc(aniCallBack);

	return true;
}

