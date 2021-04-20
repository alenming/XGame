#include "StreakAction.h"


StreakAction::StreakAction()
	: mMotionStreak(nullptr)
	, mFade(0.0f)
	, mStroke(0.0f)
{

}

StreakAction::~StreakAction()
{
	//暂时屏蔽，解决逃跑崩溃问题，不会导致内存泄漏
// 	if (mMotionStreak)
// 	{
// 		mMotionStreak->removeFromParent();
// 	}
}

StreakAction* StreakAction::create(float duration, float fade, float stroke, Point offsetPoint, const Color3B& color, const std::string& path)
{
	StreakAction* ret = new StreakAction();
	if (!ret->initWithDuration(duration, fade, stroke, offsetPoint, color, path))
	{
		delete ret;
		ret = nullptr;
	}
	else
	{
		ret->autorelease();
	}
	return ret;
}

bool StreakAction::initWithDuration(float duration, float fade, float stroke, Point offsetPoint, const Color3B& color, const std::string& path)
{
	if (ActionInterval::initWithDuration(duration))
	{
		mFade = fade;
		mStroke = stroke;
		mColor = color;
		mPath = path;
		mOffsetPoint = offsetPoint;
		mMotionStreak = MotionStreak::create(fade, 3, stroke, color, path);
		return true;
	}

	return false;
}

StreakAction* StreakAction::clone() const 
{
	return StreakAction::create(_duration, mFade, mStroke, mOffsetPoint, mColor, mPath);
}

StreakAction* StreakAction::reverse( void ) const 
{
	return StreakAction::create(_duration, mFade, mStroke, mOffsetPoint, mColor, mPath);
}

void StreakAction::startWithTarget( Node *target )
{
	ActionInterval::startWithTarget(target);
	
	//将拖尾效果添加到目标的父结点
	if (target->getParent())
	{
		mMotionStreak->setPosition(target->getPosition() + mOffsetPoint);
		BlendFunc blendFunc = {GL_SRC_ALPHA, GL_ONE};
		mMotionStreak->setBlendFunc(blendFunc);
		target->getParent()->addChild(mMotionStreak, target->getZOrder() - 1);
	}
}

void StreakAction::update( float time )
{
	if (mMotionStreak)
	{
		mMotionStreak->setPosition(_target->getPosition() + mOffsetPoint);
	}
}



