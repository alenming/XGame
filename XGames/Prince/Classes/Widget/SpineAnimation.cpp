#include "SpineAnimation.h"
#include "Utils/ResourceUtils.h"
#include "FightModule/UIFighter.h"

using namespace spine;

SpineAnimation::SpineAnimation()
	: SkeletonAnimation()
{

}

SpineAnimation::SpineAnimation( const char* skeletonDataFile, spAtlas* atlas, float scale /*= 0*/ )
	: SkeletonAnimation(skeletonDataFile, atlas, scale)
{

}

SpineAnimation::SpineAnimation( const char* skeletonDataFile, const char* atlasFile, float scale /*= 0*/ )
	: SkeletonAnimation(skeletonDataFile, atlasFile, scale)
{

}

SpineAnimation::~SpineAnimation()
{

}

SpineAnimation* SpineAnimation::createWithFile( const char* skeletonDataFile, spAtlas* atlas, float scale /*= 0*/ )
{
	SpineAnimation* node = new SpineAnimation(skeletonDataFile, atlas, scale);
	node->autorelease();
	return node;
}

SpineAnimation* SpineAnimation::createWithFile( const char* skeletonDataFile, const char* atlasFile, float scale /*= 0*/ )
{
	SpineAnimation* node = new SpineAnimation(skeletonDataFile, atlasFile, scale);
	node->autorelease();
	return node;
}

SkeletonAnimation* SpineAnimation::createWithBinaryFile(const std::string& skeletonBinaryFile, const std::string& atlasFile, float scale)
{
	SpineAnimation* skeleton = new SpineAnimation();
	skeleton->initWithBinaryFile(skeletonBinaryFile, atlasFile, 1.0f);
	skeleton->initialize();
	skeleton->autorelease();
	return skeleton;
}

SpineAnimation* SpineAnimation::createFighterWithResId(int resId)
{
	SpineAnimation* skeleton = new SpineAnimation();
	ostringstream ossBinary;
	ossBinary << ResourceUtils::getFighterSkeletonPath(resId) << resId << ".skel";
	ostringstream ossAtlas;
	ossAtlas << ResourceUtils::getFighterSkeletonPath(resId) << resId << ".atlas";
	skeleton->initWithBinaryFile(ossBinary.str(), ossAtlas.str(), 1.0f);
	skeleton->initialize();
	skeleton->autorelease();
	skeleton->setSkin(FIGHTER_SKIN_1);
	return skeleton;
}

SpineAnimation* SpineAnimation::createSkillWithResId(int resId)
{
	SpineAnimation* skeleton = new SpineAnimation();
	ostringstream ossBinary;
	ossBinary << ResourceUtils::getSkillSkeletonPath(resId) << resId << ".skel";
	ostringstream ossAtlas;
	ossAtlas << ResourceUtils::getSkillSkeletonPath(resId) << resId << ".atlas";
	skeleton->initWithBinaryFile(ossBinary.str(), ossAtlas.str(), 1.0f);
	skeleton->initialize();
	skeleton->autorelease();
	return skeleton;
}


void SpineAnimation::stopAnimation()
{
	auto callBack = [this](float dt)->void
	{
		this->pause();
		getScheduler()->unschedule("SpineAnimation::stopAnimation", this);
	};
	getScheduler()->schedule(callBack, this, 0, 0, 0, false, "SpineAnimation::stopAnimation");
	
}

void SpineAnimation::resumeAnimation()
{
	this->resume();
}

